#!/usr/bin/env python3
"""
Project Gorgon Wiki Scraper
Scrapes content from Project Gorgon wiki pages for game development research
"""

import requests
from bs4 import BeautifulSoup
import json
import time
import os
from urllib.parse import urlparse, urljoin
from pathlib import Path
import logging
from typing import Dict, List, Optional
import random

# Setup logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler('scraper.log'),
        logging.StreamHandler()
    ]
)
logger = logging.getLogger(__name__)

# Configuration
OUTPUT_DIR = Path("scraped_data")
RETRY_ATTEMPTS = 3
RETRY_DELAY = 2  # seconds
RATE_LIMIT_DELAY = 1  # seconds between requests
REQUEST_TIMEOUT = 30  # seconds

# User agents to rotate through to avoid blocking
USER_AGENTS = [
    'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36',
    'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36',
    'Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36',
    'Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:89.0) Gecko/20100101 Firefox/89.0',
    'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/14.1.1 Safari/605.1.15',
]

class ProjectGorgonScraper:
    def __init__(self, output_dir: Path = OUTPUT_DIR):
        self.output_dir = output_dir
        self.output_dir.mkdir(exist_ok=True)
        self.session = requests.Session()
        self.scraped_urls = set()
        self.failed_urls = []
        self.progress_file = self.output_dir / "progress.json"
        self.load_progress()

    def load_progress(self):
        """Load previously scraped URLs to resume if interrupted"""
        if self.progress_file.exists():
            try:
                with open(self.progress_file, 'r') as f:
                    progress = json.load(f)
                    self.scraped_urls = set(progress.get('scraped', []))
                    self.failed_urls = progress.get('failed', [])
                logger.info(f"Loaded progress: {len(self.scraped_urls)} already scraped, {len(self.failed_urls)} failed")
            except Exception as e:
                logger.warning(f"Could not load progress: {e}")

    def save_progress(self):
        """Save progress to resume later if needed"""
        try:
            with open(self.progress_file, 'w') as f:
                json.dump({
                    'scraped': list(self.scraped_urls),
                    'failed': self.failed_urls
                }, f, indent=2)
        except Exception as e:
            logger.error(f"Could not save progress: {e}")

    def get_random_headers(self) -> Dict[str, str]:
        """Get headers with random user agent"""
        return {
            'User-Agent': random.choice(USER_AGENTS),
            'Accept': 'text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8',
            'Accept-Language': 'en-US,en;q=0.5',
            'Accept-Encoding': 'gzip, deflate',
            'Connection': 'keep-alive',
            'Upgrade-Insecure-Requests': '1',
            'Sec-Fetch-Dest': 'document',
            'Sec-Fetch-Mode': 'navigate',
            'Sec-Fetch-Site': 'none',
            'Cache-Control': 'max-age=0',
        }

    def sanitize_filename(self, url: str) -> str:
        """Convert URL to safe filename"""
        parsed = urlparse(url)
        path = parsed.path.replace('/', '_').replace(':', '_')
        # Remove leading/trailing underscores
        path = path.strip('_')
        if not path:
            path = 'index'
        # Limit length and add .html extension
        return path[:200] + '.html'

    def extract_content(self, soup: BeautifulSoup, url: str) -> Dict:
        """Extract relevant content from the page"""
        data = {
            'url': url,
            'title': '',
            'main_content': '',
            'tables': [],
            'lists': [],
            'links': [],
            'images': [],
            'metadata': {}
        }

        try:
            # Title
            title_tag = soup.find('title')
            if title_tag:
                data['title'] = title_tag.get_text().strip()

            # Try to find main content area
            main_content = soup.find('div', {'id': 'content'}) or \
                          soup.find('div', {'id': 'mw-content-text'}) or \
                          soup.find('main') or \
                          soup.find('article')

            if main_content:
                # Extract text content
                data['main_content'] = main_content.get_text(separator='\n', strip=True)

                # Extract tables
                for table in main_content.find_all('table'):
                    table_data = []
                    for row in table.find_all('tr'):
                        cells = [cell.get_text(strip=True) for cell in row.find_all(['td', 'th'])]
                        if cells:
                            table_data.append(cells)
                    if table_data:
                        data['tables'].append(table_data)

                # Extract lists
                for ul in main_content.find_all(['ul', 'ol']):
                    list_items = [li.get_text(strip=True) for li in ul.find_all('li', recursive=False)]
                    if list_items:
                        data['lists'].append(list_items)

                # Extract internal links
                for link in main_content.find_all('a', href=True):
                    href = link.get('href')
                    text = link.get_text(strip=True)
                    if href and text:
                        data['links'].append({'text': text, 'href': href})

                # Extract images
                for img in main_content.find_all('img', src=True):
                    data['images'].append({
                        'src': img.get('src'),
                        'alt': img.get('alt', '')
                    })

            # Extract metadata
            for meta in soup.find_all('meta'):
                name = meta.get('name') or meta.get('property')
                content = meta.get('content')
                if name and content:
                    data['metadata'][name] = content

        except Exception as e:
            logger.error(f"Error extracting content from {url}: {e}")

        return data

    def scrape_url(self, url: str) -> Optional[Dict]:
        """Scrape a single URL with retry logic"""
        if url in self.scraped_urls:
            logger.info(f"Skipping already scraped: {url}")
            return None

        for attempt in range(RETRY_ATTEMPTS):
            try:
                logger.info(f"Scraping: {url} (attempt {attempt + 1}/{RETRY_ATTEMPTS})")

                # Add delay to be respectful
                time.sleep(RATE_LIMIT_DELAY + random.uniform(0, 0.5))

                # Make request
                response = self.session.get(
                    url,
                    headers=self.get_random_headers(),
                    timeout=REQUEST_TIMEOUT,
                    verify=False  # Disable SSL verification for problematic sites
                )

                # Check status
                if response.status_code == 200:
                    # Parse HTML
                    soup = BeautifulSoup(response.content, 'html.parser')

                    # Extract content
                    data = self.extract_content(soup, url)

                    # Save raw HTML
                    filename = self.sanitize_filename(url)
                    html_file = self.output_dir / filename
                    with open(html_file, 'w', encoding='utf-8') as f:
                        f.write(response.text)

                    # Save extracted data
                    json_file = self.output_dir / (filename.replace('.html', '.json'))
                    with open(json_file, 'w', encoding='utf-8') as f:
                        json.dump(data, f, indent=2, ensure_ascii=False)

                    # Mark as scraped
                    self.scraped_urls.add(url)
                    self.save_progress()

                    logger.info(f"✓ Successfully scraped: {url}")
                    return data

                elif response.status_code == 403:
                    logger.warning(f"403 Forbidden for {url}, trying different user agent...")
                    if attempt < RETRY_ATTEMPTS - 1:
                        time.sleep(RETRY_DELAY * (attempt + 1))
                        continue
                    else:
                        raise Exception("403 Forbidden - access denied")

                elif response.status_code == 404:
                    logger.warning(f"404 Not Found: {url}")
                    self.failed_urls.append({'url': url, 'reason': '404 Not Found'})
                    return None

                else:
                    raise Exception(f"HTTP {response.status_code}")

            except requests.exceptions.SSLError as e:
                logger.warning(f"SSL Error for {url}: {e}")
                if attempt < RETRY_ATTEMPTS - 1:
                    time.sleep(RETRY_DELAY * (attempt + 1))
                    continue

            except requests.exceptions.Timeout:
                logger.warning(f"Timeout for {url}")
                if attempt < RETRY_ATTEMPTS - 1:
                    time.sleep(RETRY_DELAY * (attempt + 1))
                    continue

            except Exception as e:
                logger.error(f"Error scraping {url}: {e}")
                if attempt < RETRY_ATTEMPTS - 1:
                    time.sleep(RETRY_DELAY * (attempt + 1))
                    continue

        # All attempts failed
        self.failed_urls.append({'url': url, 'reason': str(e) if 'e' in locals() else 'Unknown'})
        self.save_progress()
        logger.error(f"✗ Failed to scrape after {RETRY_ATTEMPTS} attempts: {url}")
        return None

    def scrape_urls(self, urls: List[str]):
        """Scrape a list of URLs"""
        logger.info(f"Starting scrape of {len(urls)} URLs")

        # Filter out already scraped URLs
        urls_to_scrape = [url for url in urls if url not in self.scraped_urls]
        logger.info(f"URLs to scrape: {len(urls_to_scrape)} (skipping {len(urls) - len(urls_to_scrape)} already scraped)")

        successful = 0
        failed = 0

        for i, url in enumerate(urls_to_scrape, 1):
            logger.info(f"\n[{i}/{len(urls_to_scrape)}] Processing: {url}")

            result = self.scrape_url(url)

            if result:
                successful += 1
            else:
                failed += 1

            # Periodic progress report
            if i % 10 == 0:
                logger.info(f"\n{'='*60}")
                logger.info(f"Progress Report: {i}/{len(urls_to_scrape)} URLs processed")
                logger.info(f"Successful: {successful}, Failed: {failed}")
                logger.info(f"{'='*60}\n")

        # Final report
        logger.info(f"\n{'='*60}")
        logger.info("SCRAPING COMPLETE")
        logger.info(f"Total URLs processed: {len(urls_to_scrape)}")
        logger.info(f"Successful: {successful}")
        logger.info(f"Failed: {failed}")
        logger.info(f"Output directory: {self.output_dir}")
        logger.info(f"{'='*60}\n")

        # Save failed URLs to file
        if self.failed_urls:
            with open(self.output_dir / 'failed_urls.json', 'w') as f:
                json.dump(self.failed_urls, f, indent=2)
            logger.info(f"Failed URLs saved to {self.output_dir / 'failed_urls.json'}")


def main():
    """Main function"""
    # Disable SSL warnings
    import urllib3
    urllib3.disable_warnings(urllib3.exceptions.InsecureRequestWarning)

    # List of URLs to scrape
    urls = [
        "https://wiki.projectgorgon.com/wiki/Black_Screen",
        "http://projectgorgon.com/",
        "https://wiki.projectgorgon.com/wiki/Game_updates/2025-10-31",
        "https://wiki.projectgorgon.com/wiki/Game_updates",
        "https://forum.projectgorgon.com/blog.php",
        "http://forum.projectgorgon.com/",
        "https://wiki.projectgorgon.com/wiki/EULA",
        "https://wiki.projectgorgon.com/wiki/Code_of_Conduct",
        "https://wiki.projectgorgon.com/wiki/User_Interface",
        "https://wiki.projectgorgon.com/wiki/Controls",
        "https://wiki.projectgorgon.com/wiki/Combat",
        "https://wiki.projectgorgon.com/wiki/Armor",
        "https://wiki.projectgorgon.com/wiki/Curse",
        "https://wiki.projectgorgon.com/wiki/Disease",
        "https://wiki.projectgorgon.com/wiki/Favor",
        "https://wiki.projectgorgon.com/wiki/Hang_Outs",
        "https://wiki.projectgorgon.com/wiki/Guilds",
        "https://wiki.projectgorgon.com/wiki/Guild_Quests",
        "https://wiki.projectgorgon.com/wiki/Items",
        "https://wiki.projectgorgon.com/wiki/Equipment",
        "https://wiki.projectgorgon.com/wiki/Transmutation",
        "https://wiki.projectgorgon.com/wiki/Augmentation",
        "https://wiki.projectgorgon.com/wiki/Treasure_Effects",
        "https://wiki.projectgorgon.com/wiki/Storage",
        "https://wiki.projectgorgon.com/wiki/Merchants",
        "https://wiki.projectgorgon.com/wiki/Bartering",
        "https://wiki.projectgorgon.com/wiki/Consignment",
        "https://wiki.projectgorgon.com/wiki/Zones",
        "https://wiki.projectgorgon.com/wiki/Dungeons",
        "https://wiki.projectgorgon.com/wiki/Quests",
        "https://wiki.projectgorgon.com/wiki/Hunting_Quests",
        "https://wiki.projectgorgon.com/wiki/Races",
        "https://wiki.projectgorgon.com/wiki/Register_of_Lore",
        "https://wiki.projectgorgon.com/wiki/Category:NPCs",
        "https://wiki.projectgorgon.com/wiki/Category:Skill_Trainer",
        "https://wiki.projectgorgon.com/wiki/Category:Creatures",
        "https://wiki.projectgorgon.com/wiki/Category:Bosses",
        "https://wiki.projectgorgon.com/wiki/Beast_Forms",
        "https://wiki.projectgorgon.com/wiki/Cosmetic_Pet",
        "https://wiki.projectgorgon.com/wiki/Words_of_Power",
        "https://wiki.projectgorgon.com/wiki/Hardcore_Mode",
        "https://wiki.projectgorgon.com/wiki/Seasonal_Event",
        "https://wiki.projectgorgon.com/wiki/Synergy_Levels",
        "https://wiki.projectgorgon.com/wiki/Level_Up_Rewards",
        "https://wiki.projectgorgon.com/wiki/Category:Guides",
        "https://wiki.projectgorgon.com/wiki/General_gameplay_tips",
        "https://wiki.projectgorgon.com/wiki/Making_Money",
        "https://wiki.projectgorgon.com/wiki/Skill_Compatibility_Chart",
        "https://wiki.projectgorgon.com/wiki/Skills",
        "https://wiki.projectgorgon.com/wiki/Category:Combat_Skills",
        "https://wiki.projectgorgon.com/wiki/Category:Beast_Skills",
        "https://wiki.projectgorgon.com/wiki/Category:Trade_Skills",
        "https://wiki.projectgorgon.com/wiki/Category:Other_Skills",
        # Combat Skills
        "https://wiki.projectgorgon.com/wiki/Animal_Handling",
        "https://wiki.projectgorgon.com/wiki/Archery",
        "https://wiki.projectgorgon.com/wiki/Bard",
        "https://wiki.projectgorgon.com/wiki/Battle_Chemistry",
        "https://wiki.projectgorgon.com/wiki/Crossbow",
        "https://wiki.projectgorgon.com/wiki/Druid",
        "https://wiki.projectgorgon.com/wiki/Fairy_Magic",
        "https://wiki.projectgorgon.com/wiki/Fire_Magic",
        "https://wiki.projectgorgon.com/wiki/Hammer",
        "https://wiki.projectgorgon.com/wiki/Ice_Magic",
        "https://wiki.projectgorgon.com/wiki/Knife_Fighting",
        "https://wiki.projectgorgon.com/wiki/Mentalism",
        "https://wiki.projectgorgon.com/wiki/Necromancy",
        "https://wiki.projectgorgon.com/wiki/Priest",
        "https://wiki.projectgorgon.com/wiki/Psychology",
        "https://wiki.projectgorgon.com/wiki/Shield",
        "https://wiki.projectgorgon.com/wiki/Staff",
        "https://wiki.projectgorgon.com/wiki/Sword",
        "https://wiki.projectgorgon.com/wiki/Unarmed",
        "https://wiki.projectgorgon.com/wiki/Vampirism",
        "https://wiki.projectgorgon.com/wiki/Weather_Witching",
        # Beast Skills
        "https://wiki.projectgorgon.com/wiki/Beast_Metabolism",
        "https://wiki.projectgorgon.com/wiki/Beast_Speech",
        "https://wiki.projectgorgon.com/wiki/Cow",
        "https://wiki.projectgorgon.com/wiki/Deer",
        "https://wiki.projectgorgon.com/wiki/Giant_Bat",
        "https://wiki.projectgorgon.com/wiki/Howling",
        "https://wiki.projectgorgon.com/wiki/Lycanthropy",
        "https://wiki.projectgorgon.com/wiki/Pig",
        "https://wiki.projectgorgon.com/wiki/Rabbit",
        "https://wiki.projectgorgon.com/wiki/Spider",
        "https://wiki.projectgorgon.com/wiki/Spirit_Fox",
        "https://wiki.projectgorgon.com/wiki/Survival_Instincts",
        "https://wiki.projectgorgon.com/wiki/Warden",
        # Trade Skills
        "https://wiki.projectgorgon.com/wiki/Alchemy",
        "https://wiki.projectgorgon.com/wiki/Cooking",
        "https://wiki.projectgorgon.com/wiki/Blacksmithing",
        "https://wiki.projectgorgon.com/wiki/Carpentry",
        "https://wiki.projectgorgon.com/wiki/Tailoring",
        "https://wiki.projectgorgon.com/wiki/Leatherworking",
        "https://wiki.projectgorgon.com/wiki/Fishing",
        "https://wiki.projectgorgon.com/wiki/Mining",
        "https://wiki.projectgorgon.com/wiki/Gardening",
        "https://wiki.projectgorgon.com/wiki/Foraging",
        "https://wiki.projectgorgon.com/wiki/Cheesemaking",
        "https://wiki.projectgorgon.com/wiki/Brewing",
        "https://wiki.projectgorgon.com/wiki/Butchering",
        # Zones
        "https://wiki.projectgorgon.com/wiki/Serbule",
        "https://wiki.projectgorgon.com/wiki/Eltibule",
        "https://wiki.projectgorgon.com/wiki/Kur_Mountains",
        "https://wiki.projectgorgon.com/wiki/Sun_Vale",
        "https://wiki.projectgorgon.com/wiki/Ilmari",
        "https://wiki.projectgorgon.com/wiki/Rahu",
        "https://wiki.projectgorgon.com/wiki/Gazluk",
        "https://wiki.projectgorgon.com/wiki/Fae_Realm",
    ]

    # Create scraper and run
    scraper = ProjectGorgonScraper()
    scraper.scrape_urls(urls)


if __name__ == "__main__":
    main()

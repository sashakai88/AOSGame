# Project Gorgon Wiki Scraper

This script scrapes content from the Project Gorgon wiki for game development research purposes.

## Features

- **Robust Error Handling**: Handles 403 errors, SSL errors, timeouts, and other common issues
- **Rate Limiting**: Respectful delays between requests to avoid overwhelming the server
- **Resume Capability**: Saves progress and can resume if interrupted
- **User Agent Rotation**: Rotates through different user agents to avoid blocking
- **Retry Logic**: Automatically retries failed requests with exponential backoff
- **Structured Data Extraction**: Extracts:
  - Page titles
  - Main content text
  - Tables
  - Lists
  - Links
  - Images
  - Metadata
- **Dual Output**: Saves both raw HTML and structured JSON data

## Installation

1. Install dependencies:
```bash
pip install -r requirements.txt
```

## Usage

### Basic Usage

Run the scraper:
```bash
python scrape_project_gorgon.py
```

The script will:
1. Create a `scraped_data/` directory
2. Download and parse each URL
3. Save HTML files and JSON data
4. Log progress to console and `scraper.log`
5. Save failed URLs to `scraped_data/failed_urls.json`

### Resuming

If the script is interrupted, just run it again. It will:
- Load `scraped_data/progress.json`
- Skip already scraped URLs
- Continue from where it left off

### Output Structure

```
scraped_data/
├── wiki_Combat.html              # Raw HTML
├── wiki_Combat.json              # Structured data
├── wiki_Skills.html
├── wiki_Skills.json
├── ...
├── progress.json                 # Progress tracker
└── failed_urls.json             # URLs that failed to scrape
```

### Structured JSON Format

Each JSON file contains:
```json
{
  "url": "https://wiki.projectgorgon.com/wiki/Combat",
  "title": "Combat - Project Gorgon Wiki",
  "main_content": "Full text content...",
  "tables": [
    [
      ["Header1", "Header2"],
      ["Data1", "Data2"]
    ]
  ],
  "lists": [
    ["Item 1", "Item 2", "Item 3"]
  ],
  "links": [
    {"text": "Link Text", "href": "/wiki/Page"}
  ],
  "images": [
    {"src": "/images/image.png", "alt": "Description"}
  ],
  "metadata": {
    "description": "Page description..."
  }
}
```

## Configuration

Edit the script constants to customize behavior:

```python
OUTPUT_DIR = Path("scraped_data")     # Output directory
RETRY_ATTEMPTS = 3                     # Number of retry attempts
RETRY_DELAY = 2                        # Initial retry delay (seconds)
RATE_LIMIT_DELAY = 1                   # Delay between requests (seconds)
REQUEST_TIMEOUT = 30                   # Request timeout (seconds)
```

## Notes

- The scraper disables SSL verification to handle SSL handshake errors
- It uses multiple user agents to avoid bot detection
- Failed URLs are logged and saved for later review
- The script is respectful of server resources with built-in rate limiting

## Troubleshooting

### 403 Forbidden Errors

The scraper handles these automatically by:
- Rotating user agents
- Retrying with different headers
- Adding delays between requests

If many URLs fail with 403, you may need to:
- Increase `RETRY_DELAY`
- Increase `RATE_LIMIT_DELAY`
- Run the script at a different time

### SSL Errors

SSL verification is disabled by default to handle SSL handshake failures.

### Timeout Errors

If you experience timeouts:
- Increase `REQUEST_TIMEOUT`
- Check your internet connection
- Try running at a different time

## Legal Note

This scraper is for personal research and educational purposes only. Always respect the website's robots.txt and terms of service. Use responsibly and ethically.

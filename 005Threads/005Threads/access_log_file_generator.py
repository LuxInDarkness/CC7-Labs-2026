import random
import time
from datetime import datetime, timedelta

# Define possible values for log entries
IP_ADDRESSES = ["192.168.1.1", "192.168.1.2", "192.168.1.3", "192.168.1.4", "192.168.1.5", "192.168.1.6", "192.168.1.7", "192.168.1.8", "192.168.1.9", "192.168.1.10"]
METHODS = ["GET", "POST", "PUT", "DELETE"]
URLS = ["/index.html", "/about.html", "/contact", "/login", "/dashboard", "/api/data", "/api/update", "/api/delete", "/products", "/services", "/blog", "/news", "/events", "/support", "/faq", "/terms", "/privacy"]
STATUS_CODES = [200, 403, 404, 500, 502, 503, 504, 405, 406, 407, 408, 409, 410, 411, 412, 413, 414, 415, 416, 417]

# Generate a starting timestamp
start_time = datetime(2024, 2, 10, 10, 20, 30)

# Open file to write logs
with open("access.log", "w") as f:
    for i in range(10000):
        ip = random.choice(IP_ADDRESSES)
        method = random.choice(METHODS)
        url = random.choice(URLS)
        status = random.choice(STATUS_CODES)

        # Increment time slightly for each log entry
        timestamp = start_time + timedelta(seconds=i)
        timestamp_str = timestamp.strftime("%d/%b/%Y:%H:%M:%S")

        # Construct log entry
        log_entry = f'{ip} - - [{timestamp_str}] "{method} {url}" {status}\n'

        # Write to file
        f.write(log_entry)

print("Generated 'access.log' with 5000 entries.")

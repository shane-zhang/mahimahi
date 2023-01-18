from subprocess import *
import os
import sys
from urllib.parse import urlparse

url = sys.argv[1]
web = urlparse(url).netloc

mmwebreplay = os.path.join(os.environ['mmpath'], 'usr/bin/mm-webreplay')
mmlink = os.path.join(os.environ['mmpath'], 'usr/bin/mm-link')
mmdelay = os.path.join(os.environ['mmpath'], 'usr/bin/mm-delay')
repo =  os.path.join(os.environ['mmpath'], 'tmp')

try:
    print ([mmwebreplay, os.path.join(repo, web), mmdelay, '0', os.path.join(repo, web), '--', 'python3', 'chrome.py', url, sys.argv[2]])
    call([mmwebreplay, os.path.join(repo, web), mmdelay, '0', os.path.join(repo, web), '--', 'browsertime', url, '-n', '1'], timeout=120, env=os.environ.copy())
except Exception as e:
    print(str(e))

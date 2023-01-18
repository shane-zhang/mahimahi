from subprocess import *
import os
import sys
import shutil
from urllib.parse import urlparse
import time


mmwebrecord = os.path.join(os.environ['mmpath'], 'usr/bin/mm-webrecord')
mmlink = os.path.join(os.environ['mmpath'], 'usr/bin/mm-link')

repo =  os.path.join(os.environ['mmpath'], 'tmp')
url = sys.argv[1]

web = urlparse(url).netloc if urlparse(url).netloc!='' else 'ftp'

try:
    #call([mmwebrecord, os.path.join(repo, web), 'python3', 'chrome.py', url, 'record'], timeout=120, env=os.environ.copy())
    #call([mmwebrecord, os.path.join(repo, web), '/opt/google/chrome/chrome', '--headless', '--user-data-dir=/tmp/nonexistent-%s'% time.time(), '--disk-cache-size=1', '--ignore-certificate-errors' , '--no-default-browser-check', '--new-window', '--no-first-run', '--disable-web-security', '--allow-running-insecure-content' , url], timeout=120, env=os.environ.copy())
    call([mmwebrecord, os.path.join(repo, web), "python3", "rr.py", url], timeout=120, env=os.environ.copy())
    print ([mmwebrecord, os.path.join(repo, web), 'google-chrome',  '--headless', '--user-data-dir=/tmp/nonexistent-%s'% time.time(), '--disk-cache-size=1', '--ignore-certificate-errors',  '--disable-web-security', '--allow-running-insecure-content' , url])
    #shutil.copyfile('tmp', os.path.join(repo, web, 'ttfb.txt'))
    call(['python3', 'parse.py', os.path.join(repo, web)])
    # call(['python3', 'transfer.py', web])
except Exception as e:
    print ("encounter exc")
    print(str(e))

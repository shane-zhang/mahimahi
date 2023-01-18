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
    #call([mmwebreplay, os.path.join(repo, web), mmdelay, '0', os.path.join(repo, web), '--', 'browsertime', url, '--chrome.args', 'headless', '-n', '1'], timeout=120, env=os.environ.copy())
    call([mmwebreplay, os.path.join(repo, web), mmdelay, '0', os.path.join(repo, web), '--', 'browsertime', url, '--xvfb', '--chrome.args', 'incognito', '--screenshot', '-n', '1'], timeout=120, env=os.environ.copy())
except Exception as e:
    print(str(e))

call(["pkill", "apache2"])
call("rm -rf /tmp/.com.google*", shell = True)

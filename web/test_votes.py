#!/usr/bin/python

import requests
import random
import time

for i in xrange(1, 100):
	nfcId = random.randint(1, 30)
	confId = random.randint(1, 10)

	requests.post("http://localhost:9000/like/%d" % confId, data={"nfcId" : nfcId})

	time.sleep(random.uniform(0.001, 0.4))

	print("vote %d done" % i)
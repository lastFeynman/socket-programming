import urllib.request
import time

if __name__ == "__main__":
    url = '192.168.44.128:8080'

    start = time.perf_counter()

    for i in range(100):
        with urllib.request.urlopen(url) as response:
            html = response.read()
            print('request ' + str(i) + ' has finished: ' + str(response.getcode()))

    end = time.perf_counter()

    print('total time: ' + str(end - start))

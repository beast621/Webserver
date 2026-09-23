import argparse
import http.client
import math
import time
from collections import Counter
from concurrent.futures import ThreadPoolExecutor


def one_request(port):
    start = time.perf_counter()
    conn = http.client.HTTPConnection("127.0.0.1", port, timeout=5)

    try:
        conn.request("GET", "/")
        response = conn.getresponse()
        body = response.read()
        code = response.status

        if code == 200 and not body:
            code = "empty body"

        return str(code), (time.perf_counter() - start) * 1000

    except (OSError, http.client.HTTPException) as error:
        return type(error).__name__, (time.perf_counter() - start) * 1000

    finally:
        conn.close()


def percentile(values, percent):
    index = math.ceil(len(values) * percent / 100) - 1
    return values[index]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--port", type=int, default=9527)
    parser.add_argument("--requests", type=int, default=5000)
    parser.add_argument("--concurrency", type=int, default=8)
    args = parser.parse_args()

    if args.requests <= 0 or args.concurrency <= 0:
        parser.error("requests and concurrency must be positive")

    with ThreadPoolExecutor(max_workers=args.concurrency) as pool:
        # 预热：让线程、文件缓存等先进入正常状态。
        list(pool.map(one_request, [args.port] * 100))

        start = time.perf_counter()
        results = list(pool.map(one_request, [args.port] * args.requests))
        seconds = time.perf_counter() - start

    codes = Counter(code for code, _ in results)
    latencies = sorted(ms for _, ms in results)

    print(f"requests={args.requests} concurrency={args.concurrency}")
    print(f"duration={seconds:.3f}s")
    print(f"throughput={args.requests / seconds:.1f} requests/s")
    print(
        f"avg={sum(latencies) / len(latencies):.2f}ms "
        f"p50={percentile(latencies, 50):.2f}ms "
        f"p95={percentile(latencies, 95):.2f}ms "
        f"p99={percentile(latencies, 99):.2f}ms"
    )
    print("results=" + ", ".join(
        f"{code}:{count}" for code, count in sorted(codes.items())
    ))

    if codes != {"200": args.requests}:
        raise SystemExit(1)


if __name__ == "__main__":
    main()
import subprocess
import json
import time

print("please run echo 1 | sudo tee /sys/devices/system/cpu/intel_pstate/no_turbo to reduce variance")

EXPERIMENTS = [
    {
        "name": "mruby",
        "trace": "/tmp/mruby_2GB",
        "image": "../test_data/mruby/data",
        "base":  0x400000,
        "end":   0x4b6000,
        "iters": 1000,
    },
    {
        "name": "unzip",
        "trace": "/tmp/unzip_2GB",
        "image": "../test_data/Unzip/data",
        "base":  0x400000,
        "end":   0x427000,
        "iters": 1000,
    },
    {
        "name": "kafl",
        "trace": "/tmp/kafl_eval_2GB",
        "image": "../test_data/kafl_eval/data.pagecache",
        "base":  0xffffffff83a00000,
        "end":    0xffffffff83a26160,
        "iters": 1000,
    },
    {
        "name": "foo",
        "trace": "/tmp/foo_2GB",
        "image": "../test_data/foo/data",
        "base":  0x7ffff7819000,
        "end":   0x7ffff79ca000,
        "iters": 1000,
    },
    {
        "name": "kernel",
        "trace": "/tmp/Kernel_2GB",
        "image": "../test_data/Kernel/data",
        "base":  0xffffffff81000000, 
        "end":   0xffffffff81e53ba0,
        "iters": 1000,
    },
    {
        "name": "avscript32",
        "trace": "/tmp/avscript_2GB",
        "image": "../test_data/avscript_32/page_cache",
        "base":  0x1000,
        "end":   0xfffff000,
        "iters": 1000,
    },

    {
        "name": "infiniteloop1",
        "trace": "/tmp/infinite_loop_linux1_2GB",
        "image": "../test_data/infinite_loop_linux/page_cache",
        "base":  0xffffffff81000000,
        "end":   0xffffffff81e54000,
        "iters": 1000,
    },
    {
        "name": "qemu",
        "trace": "/tmp/qemu1_2GB",
        "image": "../test_data/qemu/page_cache",
        "base":  0x000555555554000,
        "end":   0x00555555d60000,
        "iters": 1000,
    },
]

TOOLS = [
    {
        "name": "libxdc",
        "path": "./libxdc/tester"
    },
    {
        "name": "libipt",
        "path": "./libipt_ptxed/tester"
    },
    {
        "name": "WinAFL",
        "path": "./winaflpt/tester"
    },
    {
        "name": "PTrix",
        "path": "./ptrix/tester"
    },
    {
        "name": "killerbeez",
        "path": "./killerbeez/tester"
    },
    {
        "name": "honggfuzz",
        "path": "./honggfuzz/tester"
    }
]

NUM_RUNS = 3


for exp in EXPERIMENTS:
    res = {}
    res[exp["name"]] = {}
    tt = res[exp["name"]]
    for tool in TOOLS:
        tt[tool["name"]] = []
        tt_res = tt[tool["name"]]
        for run in range(NUM_RUNS):
            env = {
                "LD_LIBRARY_PATH":"./libipt/build/lib/:./xed/kits/xed-install-base/lib",
                "XDC_TRACE": exp["trace"],
                "XDC_IMAGE": exp["image"],
                "XDC_BASE" : str(exp["base"]),
                "XDC_END"  : str(exp["end"])
            }

            start = time.time()
            process = subprocess.run(["taskset", "-c", "0", tool["path"]], env=env, check=True)
            end = time.time()
            dur = end-start
            tt_res.append(dur)

    with open("results/results_%s.json"%(exp["name"]),"w") as f:
        json.dump(res, f)

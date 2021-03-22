import subprocess
from subprocess import PIPE
import json
import time
import io
import os

print("please run echo 1 | sudo tee /sys/devices/system/cpu/intel_pstate/no_turbo to reduce variance")
print("please run echo 0 | sudo tee /proc/sys/kernel/randomize_va_space to reduce variance")

EXPERIMENTS = [
    {
        "name":         "contrived_small_trace_1.pt",
        "trace":        "../test_data_honeybee/honeybee_unittest_data/contrived_small/trace_1.pt",
        "page_dump":    "../test_data_honeybee/small/small",
        "elf":          "../test_data_honeybee/honeybee_unittest_data/contrived_small/small",
        "base":         0x401000,
        "end":          0x402000,
        "slid":         0x401000,
        "segment":      0x1000,
    },
    {
        "name":         "contrived_small_trace_2_1.pt",
        "trace":        "../test_data_honeybee/honeybee_unittest_data/contrived_small/trace_2_1.pt",
        "page_dump":    "../test_data_honeybee/small/small",
        "elf":          "../test_data_honeybee/honeybee_unittest_data/contrived_small/small",
        "base":         0x401000,
        "end":          0x402000,
        "slid":         0x401000,
        "segment":      0x1000,
    },
    {
        "name":         "contrived_small_trace_2_2.pt",
        "trace":        "../test_data_honeybee/honeybee_unittest_data/contrived_small/trace_2_2.pt",
        "page_dump":    "../test_data_honeybee/small/small",
        "elf":          "../test_data_honeybee/honeybee_unittest_data/contrived_small/small",
        "base":         0x401000,
        "end":          0x402000,
        "slid":         0x401000,
        "segment":      0x1000,
    },
    {
        "name":         "contrived_small_trace_2_3.pt",
        "trace":        "../test_data_honeybee/honeybee_unittest_data/contrived_small/trace_2_3.pt",
        "page_dump":    "../test_data_honeybee/small/small",
        "elf":          "../test_data_honeybee/honeybee_unittest_data/contrived_small/small",
        "base":         0x401000,
        "end":          0x402000,
        "slid":         0x401000,
        "segment":      0x1000,
    },
    {
        "name":         "contrived_medium_trace_1.pt",
        "trace":        "../test_data_honeybee/honeybee_unittest_data/contrived_medium/trace_1.pt",
        "page_dump":    "../test_data_honeybee/medium/medium",
        "elf":          "../test_data_honeybee/honeybee_unittest_data/contrived_medium/medium",
        "base":         0x401000,
        "end":          0x402000,
        "slid":         0x401000,
        "segment":      0x1000,
    },
    {
        "name":         "contrived_medium_trace_2_1.pt",
        "trace":        "../test_data_honeybee/honeybee_unittest_data/contrived_medium/trace_2_1.pt",
        "page_dump":    "../test_data_honeybee/medium/medium",
        "elf":          "../test_data_honeybee/honeybee_unittest_data/contrived_medium/medium",
        "base":         0x401000,
        "end":          0x402000,
        "slid":         0x401000,
        "segment":      0x1000,
    },
    {
        "name":         "contrived_medium_trace_2_2.pt",
        "trace":        "../test_data_honeybee/honeybee_unittest_data/contrived_medium/trace_2_2.pt",
        "page_dump":    "../test_data_honeybee/medium/medium",
        "elf":          "../test_data_honeybee/honeybee_unittest_data/contrived_medium/medium",
        "base":         0x401000,
        "end":          0x402000,
        "slid":         0x401000,
        "segment":      0x1000,
    },
    {
        "name":         "contrived_medium_trace_2_3.pt",
        "trace":        "../test_data_honeybee/honeybee_unittest_data/contrived_medium/trace_2_3.pt",
        "page_dump":    "../test_data_honeybee/medium/medium",
        "elf":          "../test_data_honeybee/honeybee_unittest_data/contrived_medium/medium",
        "base":         0x401000,
        "end":          0x402000,
        "slid":         0x401000,
        "segment":      0x1000,
    },
    {
        "name":         "contrived_medium_trace_2_4.pt",
        "trace":        "../test_data_honeybee/honeybee_unittest_data/contrived_medium/trace_2_4.pt",
        "page_dump":    "../test_data_honeybee/medium/medium",
        "elf":          "../test_data_honeybee/honeybee_unittest_data/contrived_medium/medium",
        "base":         0x401000,
        "end":          0x402000,
        "slid":         0x401000,
        "segment":      0x1000,
    },
    {
        "name":         "tar_decompress_clion.pt",
        "trace":        "../test_data_honeybee/honeybee_unittest_data/tar/decompress_clion.pt",
        "page_dump":    "../test_data_honeybee/tar/tar",
        "elf":          "../test_data_honeybee/honeybee_unittest_data/tar/tar",
        "base":         0x55555555d000,
        "end":          0x5555555a4000,
        "slid":         0x55555555d000,
        "segment":      0x9000,
    },
    {
        "name":         "tar_help_page.pt",
        "trace":        "../test_data_honeybee/honeybee_unittest_data/tar/help_page.pt",
        "page_dump":    "../test_data_honeybee/tar/tar",
        "elf":          "../test_data_honeybee/honeybee_unittest_data/tar/tar",
        "base":         0x55555555d000,
        "end":          0x5555555a4000,
        "slid":         0x55555555d000,
        "segment":      0x9000,
    },
    {
        "name":         "html_fast_parse_6_txt.pt",
        "trace":        "../test_data_honeybee/honeybee_unittest_data/html_fast_parse/6_txt.pt",
        "page_dump":    "../test_data_honeybee/html_fast_parse/html_fast_parse",
        "elf":          "../test_data_honeybee/honeybee_unittest_data/html_fast_parse/fuzz_target",
        "base":         0x555555558000,
        "end":          0x55555555C000,
        "slid":         0x555555558000,
        "segment":      0x4000,
    },
    {
        "name":         "ssh_interactive_login_attempt_overflow.pt",
        "trace":        "../test_data_honeybee/honeybee_unittest_data/ssh/interactive_login_attempt_overflow.pt",
        "page_dump":    "../test_data_honeybee/ssh/ssh",
        "elf":          "../test_data_honeybee/honeybee_unittest_data/ssh/ssh",
        "base":         0x55555555e000,
        "end":          0x5555555c2000,
        "slid":         0x55555555e000,
        "segment":      0xa000,
    },
    {
        "name":         "clang_compile_simple_c_1.pt",
        "trace":        "../test_data_honeybee/honeybee_unittest_data/clang/compile_simple_c_1.pt",
        "page_dump":    "../test_data_honeybee/clang/clang",
        "elf":          "../test_data_honeybee/honeybee_unittest_data/clang/clang",
        "base":         0x400000,
        "end":          0x20cf000,
        "slid":         0x400000,
        "segment":      0x0,
    },
    {
        "name":         "clang_compile_simple_c_2.pt",
        "trace":        "../test_data_honeybee/honeybee_unittest_data/clang/compile_simple_c_2.pt",
        "page_dump":    "../test_data_honeybee/clang/clang",
        "elf":          "../test_data_honeybee/honeybee_unittest_data/clang/clang",
        "base":         0x400000,
        "end":          0x20cf000,
        "slid":         0x400000,
        "segment":      0x0,
    },
    {
        "name":         "honey_mirror_1_clang_huge.pt",
        "trace":        "../test_data_honeybee/honeybee_unittest_data/honey_mirror_1/clang_huge.pt",
        "page_dump":    "../test_data_honeybee/mirror/mirror",
        "elf":          "../test_data_honeybee/honeybee_unittest_data/honey_mirror_1/honey_mirror",
        "base":         0x401000,
        "end":          0x67e000,
        "slid":         0x401000,
        "segment":      0x1000,
    },
    {
        "name":         "honey_mirror_1_bash.pt",
        "trace":        "../test_data_honeybee/honeybee_unittest_data/honey_mirror_1/bash.pt",
        "page_dump":    "../test_data_honeybee/mirror/mirror",
        "elf":          "../test_data_honeybee/honeybee_unittest_data/honey_mirror_1/honey_mirror",
        "base":         0x401000,
        "end":          0x67e000,
        "slid":         0x401000,
        "segment":      0x1000,
    },
]


TOOLS = [
    {
        "name": "libxdc",
        "path": "./libxdc/tester",
    },
    {
        "name": "honeybee",
        "path": "./honeybee/Honeybee/cmake_build/honey_tester",
        "path_hive_gen": "./honeybee/Honeybee/cmake_build/honey_hive_generator",
    },
]

NUM_RUNS = 4

def parse_output(stdout_data):
    return_data={}

    return_data["runs"] = []

    for line in stdout_data.splitlines():
        data = line.decode("utf-8") 
        if "run_time_cold= " in data:
            return_data["cold"] = float(data.split("run_time_cold= ")[1])
        elif "run_time=      " in data:
            return_data["runs"].append(float(data.split("run_time=      ")[1]))
        elif "average=       " in data:
            return_data["average"] = float(data.split("average=       ")[1])

    return return_data["cold"], return_data["average"], return_data["runs"]


def get_avg(data):

    total = 0.0
    for d in data:
        total += d
    return total/len(data)

for exp in EXPERIMENTS:
    res = {}
    res[exp["name"]] = {}
    tt = res[exp["name"]]
    for tool in TOOLS:

        tool_name = tool["name"]
        print("%s\t%s"%(tool_name, exp["name"]))

        if tool_name == "libxdc":
            env = {
                "LD_LIBRARY_PATH":"./libipt/build/lib/:./xed/kits/xed-install-base/lib",
                "XDC_TRACE": exp["trace"],
                "XDC_IMAGE": exp["page_dump"],
                "XDC_BASE" : str(exp["base"]),
                "XDC_END"  : str(exp["end"]),
                "HONEYBEE_EVAL": "TRUE",
            }

            data = {}
            data["target"] = exp["name"]
            data["avg"] = []
            data["cold"] = []
            data["runs"] = []

            for i in range(NUM_RUNS):
                os.system("sleep 1")
                process = subprocess.run(["taskset", "-c", "0", tool["path"]], env=env, check=True, stdout=PIPE)
                cold, avg, runs = parse_output(process.stdout)
                data["avg"].append(avg)
                data["cold"].append(cold)
                data["runs"].append(runs)

            print("AVG: %f\t COLD: %f"%(get_avg(data["avg"]), get_avg(data["cold"])))

            tt[tool["name"]] = data

        elif tool_name == "honeybee":
            env = {}

            # create hive file
            process = subprocess.run(["taskset", "-c", "0", tool["path_hive_gen"], exp["elf"], "/tmp/tmp.hive"], env=env, check=True, stdout=PIPE)

            data = {}
            data["target"] = exp["name"]
            data["avg"] = []
            data["cold"] = []
            data["runs"] = []

            # run 
            for i in range(NUM_RUNS):
                os.system("sleep 1")
                process = subprocess.run(["taskset", "-c", "0", tool["path"], "-r", "-h", "/tmp/tmp.hive", "-s", hex(exp["slid"]), "-o", hex(exp["segment"]), "-t", exp["trace"], "-b", exp["elf"]], env=env, check=True, stdout=PIPE)
                cold, avg, runs = parse_output(process.stdout)
                data["avg"].append(avg)
                data["cold"].append(cold)
                data["runs"].append(runs)

            print("AVG: %f\t COLD: %f"%(get_avg(data["avg"]), get_avg(data["cold"])))
            print("===============================")

            tt[tool["name"]] = data


    with open("results_honeybee/results_%s.json"%(exp["name"]),"w") as f:
        json.dump(res, f)

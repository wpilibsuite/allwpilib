#!/usr/bin/env python3

import argparse
import multiprocessing
import os
import queue
import subprocess
import sys
from threading import Lock
from threading import Thread

from clangformat import ClangFormat
from licenseupdate import LicenseUpdate
from newline import Newline
from whitespace import Whitespace
from lint import Lint
from task import Task

# Check that the current directory is part of a Git repository
def inGitRepo(directory):
    ret = subprocess.run(["git", "rev-parse"], stderr = subprocess.DEVNULL)
    return ret.returncode == 0

def threadFunc(workQueue, isVerbose, printLock):
    # Lint is run last since previous tasks can affect its output
    tasks = [ClangFormat(), LicenseUpdate(), Newline(), Whitespace(), Lint()]

    try:
        while True:
            name = workQueue.get_nowait()

            if isVerbose:
                printLock.acquire()
                print("Processing", name,)
                for task in tasks:
                    if task.fileMatchesExtension(name):
                        print("  with " + type(task).__name__)
                printLock.release()

            for task in tasks:
                if task.fileMatchesExtension(name):
                    task.run(name)
    except queue.Empty:
        pass

def main():
    if not inGitRepo("."):
        print("Error: not invoked within a Git repository", file = sys.stderr)
        sys.exit(1)

    # Handle running in either the root or styleguide directories
    configPath = ""
    if os.getcwd().rpartition(os.sep)[2] == "styleguide":
        configPath = ".."
    else:
        configPath = "."

    # Delete temporary files from previous incomplete run
    files = [os.path.join(dp, f) for dp, dn, fn in
             os.walk(os.path.expanduser(configPath)) for f in fn]
    for f in files:
        if f.endswith(".tmp"):
            os.remove(f)

    # Recursively create list of files in given directory
    files = [os.path.join(dp, f) for dp, dn, fn in
             os.walk(os.path.expanduser(configPath)) for f in fn]

    if not files:
        print("Error: no files found to format", file = sys.stderr)
        sys.exit(1)

    # Don't check for changes in or run tasks on modifiable files
    files = [name for name in files if not Task.isModifiableFile(name)]

    # Create list of all changed files
    changedFileList = []
    proc = subprocess.Popen(["git", "diff", "--name-only", "master"],
                            bufsize = 1, stdout = subprocess.PIPE)
    for line in proc.stdout:
        changedFileList.append(configPath + os.sep +
                               line.strip().decode("ascii"))

    # Emit warning if a generated file was editted
    for name in files:
        if Task.isGeneratedFile(name) and name in changedFileList:
            print("Warning: generated file '" + name + "' modified")

    # Don't format generated files
    files = [name for name in files if not Task.isGeneratedFile(name)]

    # Parse command-line arguments
    parser = argparse.ArgumentParser(description = "Runs all formatting tasks on the code base. This should be invoked from either the styleguide directory or the root directory of the project.")
    parser.add_argument("-v", dest = "verbose", action = "store_true",
                        help = "enable output verbosity")
    parser.add_argument("-j", dest = "jobs", type = int,
                        default = multiprocessing.cpu_count(),
                        help = "number of jobs to run (default is number of cores)")
    args = parser.parse_args()
    jobs = args.jobs
    isVerbose = args.verbose

    # Add files to work queue
    workQueue = queue.Queue()
    for file in files:
        workQueue.put(file)

    threads = []
    printLock = Lock()

    # Start worker threads
    for i in range(0, jobs):
        thread = Thread(target = threadFunc,
                        args = (workQueue, isVerbose, printLock))
        thread.start()
        threads.append(thread)

    # Wait for worker threads to finish
    for i in range(0, jobs):
        threads[i].join()

if __name__ == "__main__":
    main()

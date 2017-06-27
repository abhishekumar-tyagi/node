'use strict';

const readline = require('readline');

function pad(input, minLength, fill) {
  var result = String(input);
  var padding = fill.repeat(Math.max(0, minLength - result.length));
  return `${padding}${result}`;
}

function fraction(numerator, denominator) {
  const fdenominator = String(denominator);
  const fnumerator = pad(numerator, fdenominator.length, ' ');
  return `${fnumerator}/${fdenominator}`;
}

function getTime(diff) {
  const time = Math.ceil(diff[0] + diff[1] / 1e9);
  const hours = pad(Math.floor(time / 3600), 2, '0');
  const minutes = pad(Math.floor((time % 3600) / 60), 2, '0');
  const seconds = pad((time % 3600) % 60, 2, '0');
  return `${hours}:${minutes}:${seconds}`;
}

// A run is an item in the job queue: { binary, filename, iter }
// A config is an item in the subqueue: { binary, filename, iter, configs }
class BenchmarkProgress {
  constructor(queue, benchmarks) {
    this.queue = queue;  // Scheduled runs.
    this.benchmarks = benchmarks;  // Filenames of scheduled benchmarks.
    this.completedRuns = 0;  // Number of completed runs.
    this.scheduledRuns = queue.length;  // Number of scheduled runs.
    // Time when starting to run benchmarks.
    this.startTime = process.hrtime();
    // Number of times each file will be run (roughly).
    this.runsPerFile = queue.length / benchmarks.length;
    this.currentFile = '';  // Filename of current benchmark.
    this.currentFileConfig;  // Configurations for current file
    // Number of configurations already run for the current file.
    this.completedConfig = 0;
    // Total number of configurations for the current file
    this.scheduledConfig = 0;
    this.interval = 0;  // result of setInterval for updating the elapsed time
  }

  startQueue(index) {
    this.kStartOfQueue = index;
    this.currentFile = this.queue[index].filename;
    this.interval = setInterval(() => {
      if (this.completedRuns === this.scheduledRuns) {
        clearInterval(this.interval);
      } else {
        this.updateProgress();
      }
    }, 1000);
  }

  startSubqueue(data, index) {
    // This subqueue is generated by a new benchmark
    if (data.name !== this.currentFile || index === this.kStartOfQueue) {
      this.currentFile = data.name;
      this.scheduledConfig = data.queueLength;
    }
    this.completedConfig = 0;
    this.updateProgress();
  }

  completeConfig(data) {
    this.completedConfig++;
    this.updateProgress();
  }

  completeRun(job) {
    this.completedRuns++;
    this.updateProgress();
  }

  getProgress() {
    // Get time as soon as possible.
    const diff = process.hrtime(this.startTime);

    const completedRuns = this.completedRuns;
    const scheduledRuns = this.scheduledRuns;
    const finished = completedRuns === scheduledRuns;

    // Calculate numbers for fractions.
    const runsPerFile = this.runsPerFile;
    const completedFiles = Math.floor(completedRuns / runsPerFile);
    const scheduledFiles = this.benchmarks.length;
    const completedRunsForFile =
      finished ? runsPerFile : completedRuns % runsPerFile;
    const completedConfig = this.completedConfig;
    const scheduledConfig = this.scheduledConfig;

    // Calculate the percentage.
    let runRate = 0;  // Rate of current incomplete run.
    if (completedConfig !== scheduledConfig) {
      runRate = completedConfig / scheduledConfig;
    }
    const completedRate = ((completedRuns + runRate) / scheduledRuns);
    const percent = pad(Math.floor(completedRate * 100), 3, ' ');

    const caption = finished ? 'Done\n' : this.currentFile;
    return `[${getTime(diff)}|% ${percent}| ` +
           `${fraction(completedFiles, scheduledFiles)} files | ` +
           `${fraction(completedRunsForFile, runsPerFile)} runs | ` +
           `${fraction(completedConfig, scheduledConfig)} configs]: ` +
           `${caption} `;
  }

  updateProgress(finished) {
    if (!process.stderr.isTTY || process.stdout.isTTY) {
      return;
    }
    readline.clearLine(process.stderr);
    readline.cursorTo(process.stderr, 0);
    process.stderr.write(this.getProgress());
  }
}

module.exports = BenchmarkProgress;

<p align="center">
  <img src="./docs/images/terry.jpg" alt="Terry">
</p>
<div align="right">
  <blockquote>
    I'm always counting. I'm counting right now. You blinked five times since I started talking... Six.
  </blockquote>
  <p>
    <i>- Terry, Soul (2020)</i>
  </p>
</div>

# TerryTime
[![Windows status](https://github.com/dmirell/terrytime/actions/workflows/windows-check.yaml/badge.svg)](https://github.com/dmirell/terrytime/actions/workflows/windows-check.yaml)
[![Linux status](https://github.com/dmirell/terrytime/actions/workflows/ubuntu-check.yaml/badge.svg)](https://github.com/dmirell/terrytime/actions/workflows/ubuntu-check.yaml)

## Content
- [Usage](#usage)
- [Build](#build)
- [Tests](#tests)
  - [Unit Tests](#unit-tests)
  - [Integration Tests](#integration-tests)

## Usage
General CLI structure looks like:
```bash
terry <context> <command> [args]
```
| context + *command* | arguments | description |
|-------------------|-----------|-------------|
| stream *save*       | **--symbol** - pair which market stream will be recorded<br>**--output-dir** - dir where to put output recordings<br>**--timer** - command time duration during which stream will be recording<br>*--print-stream* - if set then recorded stream will be printed in standard output | Allows to record locally market stream including trades and orderbook events locally for specific pair. |
| stream *load*       | **--stream-dir** - recorded market stream | Printing in standard output recorded market stream. |
| strategy *test*     | **--strategy** - target strategy to be tested<br>**--stream-dir** - recorded market stream for testing on<br>*--output-json-dir* - dir where to put json result, if not set then in standart output will be printed<br>*--no-ts-jump* - disables timestamp jumping optimization *(default: enabled)* | Testing target strategy on recorded market stream and outputs test result. |
| strategy *test-online* | **--strategy** - target strategy to be tested<br>**--symbol** - pair which market stream will be used for strategy test<br>**--output-dir** - dir where to put outputs<br>**--duration** - test duration | Testing target strategy. Outputs test result and recorded market stream on which strategy was tested. |
| orderbook *test*    | **--symbol** - pair on which orderbook handle will be tested | Testing local handle of orderbook in comparation with online result. Outputs result every 5 minutes in standart output. |

**Examples:**

Record market stream for symbol BTCUSDT withing 60 seconds and put result in current dir:
```bash
terry stream save --symbol=BTCUSDT --output-dir=./ --timer=60
```
Test local orderbook handle for symbol BTCUSDT:
```bash
terry orderbook test --symbol=BTCUSDT
```

## Build
1. Clone repository using SSH
```bash
git clone git@github.com:dmirell/terrytime.git
cd terrytime
```
2. Update submodules
```bash
git submodule update --init --recursive
```
4. Bootstrap vcpkg
For Windows:
```bash
.\vcpkg\bootstrap-vcpkg.bat
```
  or for Linux:
```bash
./vcpkg/bootstrap-vcpkg.sh
```
5. Run cmake
```bash
cmake -B build -S .
```
6. Run build
```bash
cmake --build build -j14
```

## Tests
### Unit tests
```bash
ctest -C <release_type>
```

### Integration tests
```bash
pytest ./integration -v --terry-path <path_to_terry_bin> --reruns 2
```

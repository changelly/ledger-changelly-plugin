# Ledger Changelly Plugin

This is a plugin for the Ethereum application which helps parsing and displaying relevant information when signing a Changelly transaction.

## Prerequisite

install Docker and docker-compose

1. `git clone https://github.com/LedgerHQ/app-ethereum && cd app-ethereum && git checkout master`
2. `git clone https://github.com/LedgerHQ/plugin-tools`
3. `git clone https://github.com/LedgerHQ/ethereum-plugin-sdk`
4. `cd plugin-tools && ./start.sh`
5. inside docker image run `cd ./app-ethereum && make`
6. to build \*.elfs for testing `cd ../tests/ && ./build_local_test_elfs.sh`

## Documentation

Need more information about the interface, the architecture, or general stuff about ethereum plugins? You can find more about them in the [ethereum-app documentation](https://developers.ledger.com/docs/dapp/nano-plugin/overview/).

## Smart Contracts

Smart contracts covered by this plugin are:

| Network  | Smart Contract Address                     |
| -------- | :----------------------------------------- |
| Ethereum | 0x1AAAd07998466cD3Eb8140827DDdb37570BE1e63 |
| BSC      | 0x6018B292fDDeAA83BB5d7B85415270B4Fc6d0C12 |
| Polygon  | 0x8e4005c5a2F85408A95adF7831F9959edA7d87d1 |

## Tests

First run: `cd tests && yarn && yarn test`

To run tests: `yarn test`

## Continuous Integration

The flow processed in GitHub Actions is the following:

Code formatting with clang-format
Compilation of the application for Ledger Nano S in ledger-app-builder

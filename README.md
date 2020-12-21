# Introduction

This repository contains simple C code which demonstrates an issue I discovered
when moving from CentOS 7 to CentOS 8.

[Blog Post](https://matthewwallace.me/posts/rhel8-fread/)

# Building

```
make
```

# Running

To demonstrate the issue:

```
./test true
```

To mitigate the issue:

```
./test false
```

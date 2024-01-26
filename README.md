OpenMM-CPP-Forces Plugin
========================

[![Linux](https://github.com/craabreu/openmm-cpp-forces/actions/workflows/Linux.yml/badge.svg)](https://github.com/craabreu/openmm-cpp-forces/actions/workflows/Linux.yml)
[![MacOS](https://github.com/craabreu/openmm-cpp-forces/actions/workflows/MacOS.yml/badge.svg)](https://github.com/craabreu/openmm-cpp-forces/actions/workflows/MacOS.yml)
[![Docs](https://github.com/craabreu/openmm-cpp-forces/actions/workflows/Docs.yml/badge.svg)](https://github.com/craabreu/openmm-cpp-forces/actions/workflows/Docs.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](https://opensource.org/licenses/MIT)

[![Conda version](https://img.shields.io/conda/v/mdtools/openmm-cpp-forces.svg)](https://anaconda.org/mdtools/openmm-cpp-forces)
[![Conda platforms](https://img.shields.io/conda/pn/mdtools/openmm-cpp-forces.svg)](https://anaconda.org/mdtools/openmm-cpp-forces)
[![Conda downloads](https://img.shields.io/conda/dn/mdtools/openmm-cpp-forces.svg)](https://anaconda.org/mdtools/openmm-cpp-forces)

This [OpenMM] plugin provides Platform-agnostic Force classes.


Available Forces
================

The Force classes implemented in OpenMM-CPP-Forces are listed in the table below.

| Force                   | Description                                                      |
|-------------------------|------------------------------------------------------------------|
| [ConcertedRMSDForce]    | RMSD of multiple structures constrained to rotate in tandem      |


Documentation
=============

Documentation for this plugin is available at [Github Pages](https://craabreu.github.io/openmm-cpp-forces/).


Installation and Usage
======================

OpenMM-CPP-Forces is available as a conda package on the
[mdtools](https://anaconda.org/mdtools/openmm-cpp-forces) channel. To install it, run:

```bash
    conda install -c conda-forge -c mdtools openmm-cpp-forces
```

Or:

```bash
    mamba install -c mdtools openmm-cpp-forces
```

To use OpenMM-CPP-Forces in your own Python script or Jupyter notebook, simply import it as follows:

```python
    import openmmcppforces
```


[OpenMM]:               https://openmm.org
[ConcertedRMSDForce]:   https://craabreu.github.io/openmm-cpp-forces/pythonapi/ConcertedRMSDForce.html

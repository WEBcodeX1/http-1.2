# Internal Class Reference / Interface Documentation

The following **Doxygen** and **Sphinx** documentation will be rendered separately.

- [Doxygen - UML relations](https://docs.webcodex.de/developer/falconas/doxygen/index.html).
- [Detailed Sphinx](https://docs.webcodex.de/developer/falconas/sphinx/index.html).

# 1. Prerequisites

The following packages (Ubuntu 22.04) need to be installed:

```bash
# install base requirements
apt-get install doxygen python3-sphinx python3-sphinx-rtd-theme python3-sphinxcontrib-mermaid
```

# 2. Continuous Integration

In future releases this process will be integrated in CI systems and triggered regularly.

# 3. Render Documentation

```bash
# run doxygen
doxygen

# run sphinx-build
sphinx-build -b html . doc_render/sphinx/
```

# 4. Rendered Content

Rendered Documentation can be found at the following locations.

## 4.1. Doxygen

`./doc_render/html/index.html`.

## 4.2. Sphinx

`./doc_render/sphinx/index.html`.

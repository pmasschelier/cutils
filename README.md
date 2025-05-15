# Kickstarter for your C projects

## Installation

### If you are using Meson

You can simply add this _cutils.wrap_ in your _subprojects_ folder:
```toml
[wrap-git]
url = https://github.com/pmasschelier/cutils
revision = HEAD
depth = 1

[provide]
cutils = cutils_dep
```

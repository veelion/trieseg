#!/usr/bin/env python
# author: veelion

import sys
import subprocess


class get_pybind_include(object):
    """Helper class to determine the pybind11 include path
    The purpose of this class is to postpone importing pybind11
    until it is actually installed, so that the ``get_include()``
    method can be invoked. """
    def __init__(self, user=False):
        try:
            import pybind11
        except ImportError:
            if subprocess.call([sys.executable, '-m', 'pip', 'install', 'pybind11']):
                raise RuntimeError('pybind11 install failed.')

        self.user = user

    def __str__(self):
        import pybind11
        return pybind11.get_include(self.user)


if __name__ == "__main__":
    from setuptools import setup, Extension

    setup(
        name='trieseg',
        version='0.1',
        author="Veelion chong",
        author_email="veelion@gmail.com",
        license='MIT',
        description=("fast tool for Chinese word segmentation using trie-tree(cedar), new-word finding, "
                     "implemented in C++, wrapped to Python"),
        packages=['trieseg'],
        package_dir={'trieseg': ''},
        ext_modules=[
            Extension(
                "trieseg",
                sources=[
                    "src/utf8.cc",
                    "src/trie_seg.cc",
                    "src/wordfinder.cc",
                    "src/wrap_pybind.cc",
                ],
                # libraries=['stdc++', ],
                include_dirs = [
                    'src',
                    get_pybind_include(),
                    get_pybind_include(user=True),
                ],
                extra_compile_args=[
                    '-std=c++14',
                    '-fPIC',
                    '-Wall',
                    '-O3',
                    '-DNDEBUG',
                ],
                language="c++"),
        ],
    )


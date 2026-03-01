from setuptools import setup, find_packages
import os
import subprocess
from setuptools.command.install import install

class CustomInstall(install):
    def run(self):
        # Build the C++ compiler during install
        compiler_dir = os.path.join(os.getcwd(), 'xbasic', 'compiler')
        subprocess.check_call(['make', '-C', os.path.dirname(compiler_dir)])
        install.run(self)

setup(
    name="xbasic-8bit",
    version="2.0.0",
    packages=find_packages(),
    include_package_data=True,
    install_requires=[],
    entry_points={
        'console_scripts': [
            'xbasic=xbasic.cli:main',
        ],
    },
    author='Vivek Dagar',
    author_email='vivekdagar@zohomail.in',
    description='XBasic — a compiled BASIC language targeting a custom 8-bit CPU.',
    url='https://github.com/alwaysvivek/xbasic',
    license='MIT',
)

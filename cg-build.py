#!/usr/bin/env python
# Most code ripped from Xenia (github.com/benvanik/xenia)

import argparse
import re
import subprocess
from subprocess import call as pcall
import sys
import os

self_path = os.path.dirname(os.path.abspath(__file__))


def main():
    # Add self to the root search path.
    sys.path.insert(0, os.path.abspath(os.path.dirname(__file__)))
    # Augment path to include our fancy things.
    os.environ['PATH'] += os.pathsep + os.pathsep.join([
        self_path,
        os.path.abspath(os.path.join('tools', 'build')),
    ])
    # Check git exists.
    if not has_bin('git'):
        print('ERROR: git must be installed and on PATH.')
        sys.exit(1)
        return
      # Check python version.
    if not sys.version_info[:2] == (2, 7):
        # TODO(benvanik): allow things to work with 3, but warn on
        # clang-format.
        print('ERROR: Python 2.7 must be installed and on PATH')
        sys.exit(1)
        return
    # Grab Visual Studio version and execute shell to set up environment.
    if sys.platform == 'win32':
        vs_version = import_vs_environment()
        if vs_version != 2015:
            print('ERROR: Visual Studio 2015 not found!')
            print('Ensure you have the VS140COMNTOOLS environment variable!')
            sys.exit(1)
            return
        # Setup main argument parser and common arguments.
    parser = argparse.ArgumentParser(prog='cg-build')
    # Grab all commands and populate the argument parser for each.
    subparsers = parser.add_subparsers(title='subcommands',
                                       dest='subcommand')
    commands = discover_commands(subparsers)
    # If the user passed no args, die nicely.
    if len(sys.argv) == 1:
        parser.print_help()
        sys.exit(1)
        return
      # Gather any arguments that we want to pass to child processes.
    command_args = sys.argv[1:]
    pass_args = []
    try:
        pass_index = command_args.index('--')
        pass_args = command_args[pass_index + 1:]
        command_args = command_args[:pass_index]
    except:
        pass

    # Parse command name and dispatch.
    args = vars(parser.parse_args(command_args))
    command_name = args['subcommand']
    try:
        command = commands[command_name]
        return_code = command.execute(args, pass_args, os.getcwd())
    except Exception as e:
        raise
        return_code = 1
    sys.exit(return_code)


def import_vs_environment():
    """Finds the installed Visual Studio version and imports
    interesting environment variables into os.environ.

    Returns:
        A version such as 2015 or None if no VS is found.
    """
    version = 0
    tools_path = ''
    if 'VS140COMNTOOLS' in os.environ:
        version = 2015
        tools_path = os.environ['VS140COMNTOOLS']
    if version == 0:
        return None
    tools_path = os.path.join(tools_path, '..\\..\\vc\\vcvarsall.bat')

    args = [tools_path, '&&', 'set']
    popen = subprocess.Popen(
        args, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    variables, _ = popen.communicate()
    envvars_to_save = (
        'devenvdir',
        'include',
        'lib',
        'libpath',
        'path',
        'pathext',
        'systemroot',
        'temp',
        'tmp',
        'windowssdkdir',
    )
    for line in variables.splitlines():
        for envvar in envvars_to_save:
            if re.match(envvar + '=', line.lower()):
                var, setting = line.split('=', 1)
                if envvar == 'path':
                    setting = os.path.dirname(
                        sys.executable) + os.pathsep + setting
                os.environ[var.upper()] = setting
                break

    os.environ['VSVERSION'] = str(version)
    return version


def has_bin(bin):
    """Checks whether the given binary is present.

    Args:
        bin: binary name (without .exe, etc).

    Returns:
        True if the binary exists.
    """
    bin_path = get_bin(bin)
    if not bin_path:
        return False
    return True


def get_bin(bin):
    """Checks whether the given binary is present and returns the path.

    Args:
        bin: binary name (without .exe, etc).

    Returns:
        Full path to the binary or None if not found.
    """
    for path in os.environ['PATH'].split(os.pathsep):
        path = path.strip('"')
        exe_file = os.path.join(path, bin)
        if os.path.isfile(exe_file) and os.access(exe_file, os.X_OK):
            return exe_file
        exe_file = exe_file + '.exe'
        if os.path.isfile(exe_file) and os.access(exe_file, os.X_OK):
            return exe_file
    return None


def git_submodule_update():
    """Runs a full recursive git submodule init and update.

    Older versions of git do not support 'update --init --recursive'. We could
    check and run it on versions that do support it and speed things up a bit.
    """
    if True:
        pcall([
            'git',
            'submodule',
            'update',
            '--init',
            '--recursive',
        ])
    else:
        pcall([
            'git',
            'submodule',
            'init',
        ])
        pcall([
            'git',
            'submodule',
            'foreach',
            '--recursive',
            'git',
            'submodule',
            'init',
        ])
        pcall([
            'git',
            'submodule',
            'update',
            '--recursive',
        ])


def run_premake(target_os, action):
    """Runs premake on the main project with the given format.

    Args:
        target_os: target --os to pass to premake.
        action: action to preform.
    """
    ret = subprocess.call([
        'python',
        os.path.join('tools', 'build', 'premake'),
        '--file=premake5.lua',
        '--os=%s' % (target_os),
        '--cc=clang',
        '--test-suite-mode=combined',
        '--verbose',
        action,
    ], shell=False)

    #if ret == 0:
    #    generate_version_h()

    return ret


def run_premake_clean():
    """Runs a premake clean operation.
    """
    if sys.platform == 'darwin':
        return run_premake('macosx', 'clean')
    elif sys.platform == 'win32':
        return run_premake('windows', 'clean')
    else:
        return run_premake('linux', 'clean')


def run_platform_premake():
    """Runs all gyp configurations.
    """
    if sys.platform == 'darwin':
        return run_premake('macosx', 'xcode4')
    elif sys.platform == 'win32':
        return run_premake('windows', 'vs2015')
    else:
        ret = run_premake('linux', 'gmake')
        ret = ret != 0 and run_premake('linux', 'codelite') or ret
        return ret


def discover_commands(subparsers):
    commands = {
        'setup': SetupCommand(subparsers),
        'premake': PremakeCommand(subparsers),
        'build': BuildCommand(subparsers),
    }

    return commands


class Command(object):
    """Base type for commands.
    """

    def __init__(self, subparsers, name, help_short=None, help_long=None,
                 *args, **kwargs):
        """Initializes a command.

        Args:
        subparsers: Argument subparsers parent used to add command parsers.
        name: The name of the command exposed to the management script.
        help_short: Help text printed alongside the command when queried.
        help_long: Extended help text when viewing command help.
        """
        self.name = name
        self.help_short = help_short
        self.help_long = help_long

        self.parser = subparsers.add_parser(name,
                                            help=help_short,
                                            description=help_long)
        self.parser.set_defaults(command_handler=self)

    def execute(self, args, pass_args, cwd):
        """Executes the command.

        Args:
        args: Arguments hash for the command.
        pass_args: Arguments list to pass to child commands.
        cwd: Current working directory.

        Returns:
        Return code of the command.
        """
        return 1


class SetupCommand(Command):
    """'setup' command."""

    def __init__(self, subparsers, *args, **kwargs):
        super(SetupCommand, self).__init__(
            subparsers,
            name='setup',
            help_short='Setup the build environment.',
            *args, **kwargs)

    def execute(self, args, pass_args, cwd):
        print('Setting up the build environment...')
        print('')

        # Setup submodules.
        print('- git submodule init / update...')
        git_submodule_update()
        print('')

        print('- running premake...')
        if run_platform_premake() == 0:
            print('')
            print('Success!')

        return 0


class PremakeCommand(Command):
    """'premake' command."""

    def __init__(self, subparsers, *args, **kwargs):
        super(PremakeCommand, self).__init__(
            subparsers,
            name='premake',
            help_short='Runs premake to update all projects.',
            *args, **kwargs)

    def execute(self, args, pass_args, cwd):
        # Update premake. If no binary found, it will be built from source.
        print('Running premake...')
        print('')
        if run_platform_premake() == 0:
            print('Success!')

        return 0


class BaseBuildCommand(Command):
    """Base command for things that require building."""

    def __init__(self, subparsers, *args, **kwargs):
        super(BaseBuildCommand, self).__init__(
            subparsers,
            *args, **kwargs)
        self.parser.add_argument(
            '--config', choices=['checked', 'debug', 'release'], default='debug',
            help='Chooses the build configuration.')
        self.parser.add_argument(
            '--target', action='append', default=[],
            help='Builds only the given target(s).')
        self.parser.add_argument(
            '--force', action='store_true',
            help='Forces a full rebuild.')
        self.parser.add_argument(
            '--no_premake', action='store_true',
            help='Skips running premake before building.')

    def execute(self, args, pass_args, cwd):
        if not args['no_premake']:
            print('- running premake...')
            run_platform_premake()
            print('')

        print('- building (%s):%s...' % (
            'all' if not len(args['target']) else ', '.join(args['target']),
            args['config']))
        if sys.platform == 'win32':
            targets = None
            if len(args['target']):
                targets = '/t:' + ';'.join(target + (':Rebuild' if args['force'] else '')
                                           for target in args['target'])
            else:
                targets = '/t:Rebuild' if args['force'] else None

            result = subprocess.call([
                'msbuild',
                'build/cgraphics.sln',
                '/nologo',
                '/m',
                '/v:m',
                '/p:Configuration=' + args['config'],
            ] + ([targets] if targets is not None else []) + pass_args, shell=False)
        elif sys.platform == 'darwin':
            # TODO(benvanik): other platforms.
            print('ERROR: don\'t know how to build on this platform.')
        else:
            # TODO(benvanik): allow gcc?
            if 'CXX' not in os.environ:
                os.environ['CXX'] = 'clang++-3.8'
            if 'CC' not in os.environ:
                os.environ['CC'] = 'clang-3.8'

            result = subprocess.call([
                'make',
                '-Cbuild/',
                'config=%s_linux' % (args['config']),
            ] + pass_args + args['target'], shell=False)
        print('')
        if result != 0:
            print('ERROR: build failed with one or more errors.')
            return result
        return 0


class BuildCommand(BaseBuildCommand):
    """'build' command."""

    def __init__(self, subparsers, *args, **kwargs):
        super(BuildCommand, self).__init__(
            subparsers,
            name='build',
            help_short='Builds the project.',
            *args, **kwargs)

    def execute(self, args, pass_args, cwd):
        print('Building %s...' % (args['config']))
        print('')

        result = super(BuildCommand, self).execute(args, pass_args, cwd)
        if not result:
            print('Success!')
        return result

if __name__ == '__main__':
    main()

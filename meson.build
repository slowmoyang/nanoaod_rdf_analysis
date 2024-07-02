project(
  'nanoaod_rdf_analysis',
  'cpp',
  license: 'MIT',
  version: '1.0.0',
)

# root
root_config = find_program('root-config', required: true)
root_compile_args = run_command(root_config, '--cflags', check: true).stdout().strip().split(' ')
root_link_args = run_command(root_config, '--ldflags', '--libs', check: true).stdout().strip().split(' ')
root_incdir = run_command(root_config, '--incdir', check: true).stdout().strip().split(' ')
root_dep = declare_dependency(
    compile_args: root_compile_args,
    link_args: root_link_args,
    include_directories: root_incdir
)

# correctionlib
correction = find_program('correction', required: true)
correctionlib_compile_args = run_command(correction, 'config', '--cflags', check: true).stdout().strip().split(' ')
correctionlib_link_args = run_command(correction, 'config', '--ldflags', '--rpath', check: true).stdout().strip().split(' ')
correctionlib_incdir = run_command(correction, 'config', '--incdir', check: true).stdout().strip().split(' ')
correctionlib_dep = declare_dependency(
  compile_args: correctionlib_compile_args,
  link_args: correctionlib_link_args,
  include_directories: correctionlib_incdir
)

rdf_analysis_dep = dependency(
  'rdf_analysis',
  required: true,
  static: true,
)

nanoaod_tools_dep = dependency(
  'nanoaod_tools',
  required: true,
  static: true,
  # fallback: ['nanoaod_tools', 'nanoaod_tools_dep'],
)

# https://fmt.dev/latest/usage.html
fmt = subproject('fmt', default_options: 'default_library=static')
fmt_dep = fmt.get_variable('fmt_dep')


sources = [
  './src/correction_helper.cc',
  './src/parton_jet_matching.cc',
  './src/selection.cc',
  './src/utilc.cc',
]

nanoaod_rdf_analysis_dep = declare_dependency(
  include_directories: include_directories('include'),
  sources: sources,
  dependencies: [
    root_dep,
    correctionlib_dep,
    nanoaod_tools_dep,
    rdf_analysis_dep,
    fmt_dep,
  ],
)

meson.override_dependency(meson.project_name(), nanoaod_rdf_analysis_dep)

require 'rake/builder'

ARCHITECTURE = 'x86_64'

PROFILED     = ENV['PROFILED']

name          = "#{ARCHITECTURE}"
name          += '_profiled'                  if PROFILED

def running_tests
  app = Rake.application
  app.top_level_tasks.any?{ | task | task =~ /^test(\W|$)/ }
end

raise "Supply a PG_USER for tests" if running_tests && ENV[ 'PG_USER' ].nil?

if ENV['TEST_FILES']
  TEST_NAME                = "partial_#{name}_test"
  TEST_SOURCE_SEARCH_PATHS = [ 'test/main.cpp', 'test/test_helper.cpp' ] + ENV[ 'TEST_FILES' ].split( ',' ).map{ | f | "test/#{f}" }
else
  TEST_NAME                = "#{name}_test"
  TEST_SOURCE_SEARCH_PATHS = [ 'test', 'test/connection' ]
end

desc 'Print help information for this Rakefile'
task :help do
  puts <<-EOT
    Build for profiling:
      $ PROFILED=1 rake ...
    When running tests, set up postgresql:
    - arrange for password-less access (via .pg_pass)
    - pass the name of a user on the command line:
      $ PG_USER=foo rake test:run
      the user must have the necessary privileges to create databases
    Test only certain files:
      $ PG_USER=foo TEST_FILES=foo_test.cpp,bar_test.cpp rake test:run
   EOT
end

Rake::Builder.new do | builder |
  builder.target               = "libactive_record_#{ name }.a"
  builder.architecture         = ARCHITECTURE
  builder.source_search_paths  = [ 'src', 'src/connection' ]
  builder.installable_headers  = [ 'include/**/*.h' ]
  builder.objects_path         = "objects/#{ name }"
  builder.include_paths        = [ 'include', '/usr/include/postgresql/9.1/server', '/usr/include/postgresql' ] # TODO: get pg includes from pg_config
  builder.compilation_options  = [ '-pg' ] if PROFILED
end

task :default => 'build'

Rake::Builder.new do | builder |
  builder.task_namespace       = :test
  builder.target               = "./active_record_#{ TEST_NAME }"
  builder.architecture         = ARCHITECTURE
  builder.source_search_paths  = TEST_SOURCE_SEARCH_PATHS
  builder.installable_headers  = [ 'test' ]
  builder.objects_path         = "test/objects/#{ TEST_NAME }"
  builder.include_paths        = [ 'include', 'test' ]
  builder.linker_options       = [ '-L.' ]
  builder.library_dependencies = [ 'gtest', "active_record_#{ name }", 'pq', 'sqlite3' ]
  builder.library_paths        = [ "." ]
  builder.target_prerequisites = [ :"rake:build" ]
  builder.default_task         = :run
  builder.compilation_options  = [ "-DPG_USER=#{ ENV[ 'PG_USER' ] }" ]
end


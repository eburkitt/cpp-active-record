require 'rake/builder'

DEFAULT_DATABASE     = 'sqlite3'
DEFAULT_ARCHITECTURE = 'x86_64'

CONFIGURATIONS = [
  { :database => 'postgresql', :architecture => 'i386',   :libs => [ 'pq' ] },
  { :database => 'sqlite3',    :architecture => 'i386',   :libs => [ 'sqlite3' ] },
  { :database => 'sqlite3',    :architecture => 'x86_64', :libs => [ 'sqlite3' ] },
]

DATABASE     = ENV['DATABASE']     || DEFAULT_DATABASE
ARCHITECTURE = ENV['ARCHITECTURE'] || DEFAULT_ARCHITECTURE
PROFILED     = ENV['PROFILED']

name          = "#{DATABASE}_#{ARCHITECTURE}"
name          += '_profiled'                  if PROFILED

configuration = CONFIGURATIONS.find { | c | c[ :database ] == DATABASE && c[ :architecture ] == ARCHITECTURE }
raise "Unknown configuration: DATABASE=#{DATABASE} ARCHITECTURE=#{ARCHITECTURE}" if configuration.nil?

if ENV['TEST_FILES']
  TEST_SOURCE_SEARCH_PATHS = ENV['TEST_FILES'].split(',').map{ | f | 'test/#{f}'}
  TEST_OBJECTS_PATH        = "#{ name }_partial_test"
else
  TEST_SOURCE_SEARCH_PATHS = [ 'test' ]
  TEST_OBJECTS_PATH        = "#{ name }_test"
end

desc 'Print help information for this Rakefile'
task :help do
  puts <<-EOT
    List available target databases and processor architectures:
      $ rake list_configurations
    Specify target database and processor architecture:
      $ DATABASE=postgresql ARCHITECTURE=i386 rake
    Defaults are:
      DATABASE=#{DEFAULT_DATABASE}
      ARCHITECTURE=#{DEFAULT_ARCHITECTURE}
    Build for profiling:
      $ PROFILED=1 rake
    Test only certain files:
      $ TEST_FILES=foo_test.cpp,bar_test.cpp
   EOT
end

desc 'List available configurations'
task :list_configurations do
  puts 'DATABASE            | ARCHITECTURE'
  puts '--------------------+-----------------------'
  CONFIGURATIONS.each do | configuration |
    puts '%- 19s | %s' % [ configuration[ :database ], configuration[ :architecture ] ]
  end
end

Rake::Builder.new do | builder |
  builder.task_namespace       = name.intern
  builder.target               = "libactive_record_#{ name }.a"
  builder.architecture         = ARCHITECTURE
  builder.source_search_paths  = [ 'src' ]
  builder.installable_headers  = [ 'include/**/*.h' ]
  builder.objects_path         = "objects/#{ name }"
  builder.include_paths        = [ 'include' ]
  builder.library_dependencies = configuration[ :libs ]
  builder.compilation_options  = [ '-pg' ] if PROFILED
end

task :default => name

Rake::Builder.new do | builder |
  builder.task_namespace       = :test
  builder.target               = "./active_record_#{ name }_test"
  builder.architecture         = ARCHITECTURE
  builder.source_search_paths  = TEST_SOURCE_SEARCH_PATHS
  builder.installable_headers  = [ 'test' ]
  builder.objects_path         = "test/objects/#{ TEST_OBJECTS_PATH }"
  builder.include_paths        = [ 'include', 'test' ]
  builder.linker_options       = [ '-L.' ]
  builder.library_dependencies = [ 'gtest', "active_record_#{ name }" ] + configuration[ :libs ]
  builder.library_paths        = [ "." ]
  builder.target_prerequisites = [ :"rake:build" ]
  builder.default_task         = :run
  builder.compilation_options  = [ "-DPG_USER=#{ ENV[ 'PG_USER' ] }" ]
end


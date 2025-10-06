#!/usr/bin/ruby

# This file defines configuration options to be used instead of hardcoded values.
# The version of Qt we use needs to be accessed by both CMake and ruby build_dist script etc..

# The config options

$vs_version = 2022 # Visual Studio version used to build libraries and Substrata.  Used in build.rb and library build scripts.


# 5.15.16 is the latest released open source version in the pre-6.0 series as of November 2024.
# NOTE: should match the qt_version in substrata-private config-lib.rb.
$qt_version = "5.15.16" if OS.windows?
$qt_version = "5.15.10" if OS.mac? 
$qt_version = "5.15.10" if OS.linux?


$llvm_version = "15.0.7" # NOTE: Also defined in SUBSTRATA_LLVM_VERSION in CMakeLists.txt.


# Get Qt path.
glare_core_libs_dir = ENV['GLARE_CORE_LIBS']
if glare_core_libs_dir.nil?
	puts "GLARE_CORE_LIBS env var not defined."
	exit(1)
end

indigo_qt_base_dir = "#{glare_core_libs_dir}/Qt"

$indigo_qt_dir = ""
if OS.unix?
	$indigo_qt_dir = "#{indigo_qt_base_dir}/#{$qt_version}"
else
	$indigo_qt_dir = "#{indigo_qt_base_dir}/#{$qt_version}-vs#{$vs_version}-64"
end


def get_substrata_version
	versionfile = IO.readlines("../shared/Version.h").join

	versionmatch = versionfile.match(/cyberspace_version\s+=\s+\"(.*)\"/)

	if versionmatch.nil? || versionmatch[1].nil?
		puts "Failed to extract version number from Version.h"
		exit(1)
	end

	version = versionmatch[1]
	version
end


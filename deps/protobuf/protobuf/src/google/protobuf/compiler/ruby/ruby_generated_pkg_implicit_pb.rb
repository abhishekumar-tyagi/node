# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: ruby_generated_pkg_implicit.proto

require 'google/protobuf'

Google::Protobuf::DescriptorPool.generated_pool.build do
  add_file("ruby_generated_pkg_implicit.proto", :syntax => :proto3) do
    add_message "one.two.a_three.Four" do
      optional :a_string, :string, 1
    end
  end
end

module One
  module Two
    module AThree
      Four = Google::Protobuf::DescriptorPool.generated_pool.lookup("one.two.a_three.Four").msgclass
    end
  end
end

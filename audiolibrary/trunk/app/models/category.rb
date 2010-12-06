class Category < ActiveRecord::Base

  has_and_belongs_to_many :audio_items
  belongs_to :parent, :class_name => "Category", :foreign_key => "parent_id"
  has_many :children, :class_name => "Category", :foreign_key => "parent_id"
end

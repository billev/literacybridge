class AudioItem < ActiveRecord::Base
  has_many :localizations, :dependent => :destroy
  has_and_belongs_to_many :categories
  
  accepts_nested_attributes_for :localizations

  attr_accessor :category_list
  after_save :update_categories

  private

  def update_categories
    logger.debug ">>>>>>Updating categories!"
    categories.delete_all
    selected_categories = category_list.nil? ? [] : category_list.keys.collect{|id| Category.find_by_id(id)}
    selected_categories.each {|category| self.categories << category }
  end
    
end

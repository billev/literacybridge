class AddJoinTableForCategories < ActiveRecord::Migration
  def self.up
    create_table :audio_items_categories, :id => false do |t|
      t.integer :audio_item_id
      t.integer :category_id
      t.timestamps
    end
  end

  def self.down
    drop_table :audio_items_categories
  end
end

class AddCategoroyToAudioItem < ActiveRecord::Migration
  def self.up
    add_column :audio_items, :category_id, :integer
  end

  def self.down
    remove_column :audio_items, :category_id
  end
end

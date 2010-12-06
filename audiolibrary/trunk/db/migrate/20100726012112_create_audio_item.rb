class CreateAudioItem < ActiveRecord::Migration
  def self.up
    create_table :audio_items do |t|
      t.string "uuid" #Universal identifier for use outside the application; separate from table ID
      
      t.timestamps
    end
    
    create_table :localizations do |t|
      t.string  "contributor"
      t.string  "coverage"
      t.string  "creator",               :limit => 100
      t.date    "date"
      t.string  "description",           :limit => 2048
      t.string  "format",                :limit => 50
      t.string  "identifier",            :limit => 50
      t.integer "language"
      t.string  "publisher",             :limit => 100
      t.string  "relation"
      t.string  "rights"
      t.string  "source"
      t.string  "subject"
      t.string  "title"
      t.string  "type",                    :limit => 50
      t.string  "revision",               :limit => 50
      t.date    "revision_date"
      t.string  "revision_description",   :limit => 2048
      t.integer "lb_rating",                :limit => 2
      t.integer "locale_id"
      t.integer "audio_item_id"
      
      t.timestamps
    end
    
    create_table :locales do |t|
      t.string "language"
      t.string "country",     :limit => 100
      t.string "description", :limit => 2048
      
      t.timestamps
    end
    
    create_table :statistics do |t|
      t.integer "localization_id"
      t.string  "device_id"
      t.integer "copy_count"
      t.integer "play_count"
      
      t.timestamps
    end
  end

  def self.down
    drop_table :audio_items
    drop_table :localizations
    drop_table :locales
    drop_table :statistics
  end
end

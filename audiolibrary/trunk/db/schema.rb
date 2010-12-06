# This file is auto-generated from the current state of the database. Instead of editing this file, 
# please use the migrations feature of Active Record to incrementally modify your database, and
# then regenerate this schema definition.
#
# Note that this schema.rb definition is the authoritative source for your database schema. If you need
# to create the application database on another system, you should be using db:schema:load, not running
# all the migrations from scratch. The latter is a flawed and unsustainable approach (the more migrations
# you'll amass, the slower it'll run and the greater likelihood for issues).
#
# It's strongly recommended to check this file into your version control system.

ActiveRecord::Schema.define(:version => 20101206010052) do

  create_table "audio_items", :force => true do |t|
    t.string   "uuid"
    t.datetime "created_at"
    t.datetime "updated_at"
    t.integer  "category_id"
  end

  create_table "audio_items_categories", :id => false, :force => true do |t|
    t.integer  "audio_item_id"
    t.integer  "category_id"
    t.datetime "created_at"
    t.datetime "updated_at"
  end

  create_table "categories", :force => true do |t|
    t.datetime "created_at"
    t.datetime "updated_at"
    t.string   "name"
    t.integer  "parent_id"
  end

  create_table "locales", :force => true do |t|
    t.string   "language"
    t.string   "country",     :limit => 100
    t.string   "description", :limit => 2048
    t.datetime "created_at"
    t.datetime "updated_at"
  end

  create_table "localizations", :force => true do |t|
    t.string   "contributor"
    t.string   "coverage"
    t.string   "creator",                :limit => 100
    t.date     "date"
    t.string   "description",            :limit => 2048
    t.string   "format",                 :limit => 50
    t.string   "identifier",             :limit => 50
    t.integer  "language"
    t.string   "publisher",              :limit => 100
    t.string   "relation"
    t.string   "rights"
    t.string   "source"
    t.string   "subject"
    t.string   "title"
    t.string   "type",                   :limit => 50
    t.string   "revision",               :limit => 50
    t.date     "revision_date"
    t.string   "revision_description",   :limit => 2048
    t.integer  "lb_rating",              :limit => 2
    t.integer  "locale_id"
    t.integer  "audio_item_id"
    t.datetime "created_at"
    t.datetime "updated_at"
    t.string   "recording_file_name"
    t.string   "recording_content_type"
    t.integer  "recording_file_size"
    t.datetime "recording_updated_at"
  end

  create_table "statistics", :force => true do |t|
    t.integer  "localization_id"
    t.string   "device_id"
    t.integer  "copy_count"
    t.integer  "play_count"
    t.datetime "created_at"
    t.datetime "updated_at"
  end

end

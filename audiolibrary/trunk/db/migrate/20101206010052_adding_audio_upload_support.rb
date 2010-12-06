class AddingAudioUploadSupport < ActiveRecord::Migration
  def self.up
        add_column :localizations, :recording_file_name,    :string
        add_column :localizations, :recording_content_type, :string
        add_column :localizations, :recording_file_size,    :integer
        add_column :localizations, :recording_updated_at,   :datetime
      end

      def self.down
        remove_column :localizations, :recording_file_name
        remove_column :localizations, :recording_content_type
        remove_column :localizations, :recording_file_size
        remove_column :localizations, :recording_updated_at
      end
end

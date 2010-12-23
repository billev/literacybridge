class Localization < ActiveRecord::Base

  require 'paperclip'

  belongs_to :locale
  belongs_to :audio_item
  validates_presence_of :title
  validates_attachment_presence :recording
  
  has_attached_file :recording,
    :storage => :s3,
    :whiny => false,
    :path => "public/images/:attachment/:id/:file_name.:extension",
    :s3_credentials => "config/amazon_s3.yml",
    :bucket => AMAZON_BUCKET
end

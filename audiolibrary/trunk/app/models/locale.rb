class Locale < ActiveRecord::Base

  has_many :localizations

  validates_presence_of :language, :country, :description
end

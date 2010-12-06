module CategoriesHelper

  def facet_string(c)
    c.name + " (" + c.audio_items.size.to_s + ")"
  end

end

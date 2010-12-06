class MainController < ApplicationController
  
  def index
    @audio_items = AudioItem.all(:include => [ { :localizations => [ :locale ] }  ])
    @categories = Category.all

    respond_to do |format|
      format.html # index.html.erb
      format.xml  { render :xml => @audio_items }
    end
  end
  
end

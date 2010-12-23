class AudioItemsController < ApplicationController
  # GET /audio_items
  # GET /audio_items.xml
  def index
    @categories = Category.all(:conditions => { :parent_id => nil })
    
    if params[:category_id]
      @category = Category.find(params[:category_id])
      @audio_items = AudioItem.all(:include => [ { :localizations => [ :locale ] } ])
      @audio_items.reject! { |a| !(a.categories.include? @category) } 
    else
      @audio_items = AudioItem.all(:include => [ { :localizations => [ :locale ] }  ])
    end
    

    respond_to do |format|
      format.html # index.html.erb
      format.xml  { render :xml => @audio_items }
    end
  end

  # GET /audio_items/1
  # GET /audio_items/1.xml
  def show
    @audio_item = AudioItem.find(params[:id])
    @categories = Category.find(:all)
    
    if params[:localization_id]
      @default_localization = Localization.find(params[:localization_id]) 
    end
    @default_localization ||= @audio_item.localizations.first

    respond_to do |format|
      format.html # show.html.erb
      format.xml  { render :xml => @audio_item }
    end
  end

  # GET /audio_items/new
  # GET /audio_items/new.xml
  def new
    @audio_item = AudioItem.new
    @audio_item.localizations << Localization.new
    @categories = Category.find(:all)

    respond_to do |format|
      format.html # new.html.erb
      format.xml  { render :xml => @audio_item }
    end
  end

  # GET /audio_items/1/edit
  def edit
    @audio_item = AudioItem.find(params[:id])
    @categories = Category.find(:all)
    
    if params[:localization_id]
      @default_localization = Localization.find(params[:localization_id]) 
    end
    @default_localization ||= @audio_item.localizations.first
  end

  # POST /audio_items
  # POST /audio_items.xml
  def create
    @audio_item = AudioItem.new(params[:audio_item])
    @categories = Category.find(:all)

    respond_to do |format|
      if @audio_item.save
        format.html { redirect_to(@audio_item, :notice => 'AudioItem was successfully created.') }
        format.xml  { render :xml => @audio_item, :status => :created, :location => @audio_item }
      else
        logger.error "Errors: " + @audio_item.errors.to_s
        format.html { render :action => "new" }
        format.xml  { render :xml => @audio_item.errors, :status => :unprocessable_entity }
      end
    end
  end

  # PUT /audio_items/1
  # PUT /audio_items/1.xml
  def update
    @audio_item = AudioItem.find(params[:id])
    @audio_item.save

    respond_to do |format|
      if @audio_item.update_attributes(params[:audio_item])

        format.html { redirect_to(@audio_item, :notice => 'AudioItem was successfully updated.') }
        format.xml  { head :ok }
      else
        format.html { render :action => "edit" }
        format.xml  { render :xml => @audio_item.errors, :status => :unprocessable_entity }
      end
    end
  end

  # DELETE /audio_items/1
  # DELETE /audio_items/1.xml
  def destroy
    @audio_item = AudioItem.find(params[:id])
    @audio_item.destroy

    respond_to do |format|
      format.html { redirect_to(audio_items_url) }
      format.xml  { head :ok }
    end
  end
end

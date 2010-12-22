# Filters added to this controller apply to all controllers in the application.
# Likewise, all the methods added will be available for all controllers.

class ApplicationController < ActionController::Base
  helper :all # include all helpers, all the time
  protect_from_forgery # See ActionController::RequestForgeryProtection for details
  before_filter :identify_browser, :identify_os

  # Scrub sensitive parameters from your log
  # filter_parameter_logging :password
  
  def identify_browser
    user_agent = request.env['HTTP_USER_AGENT']
    
    if user_agent
      if user_agent.index('MSIE 8')
        @browser = 'ie8 ie'
      elsif user_agent.index('MSIE 7')
        @browser = 'ie7 ie'
      elsif user_agent.index('MSIE 6')
        @browser = 'ie6 ie'
      elsif user_agent.index('Firefox/2')
        @browser = 'ff2 ff'
      elsif user_agent.index('Firefox/3')
        @browser = 'ff3 ff'
      elsif user_agent.index('Chrome')
        @browser = 'chrome'
      elsif user_agent.index('Safari/528')
        @browser = 'safari4 safari'
      elsif user_agent.index('Safari')
        @browser = 'safari'
      else
        @browser = 'other_browsers'
      end
    end
  end
    
  def identify_os
    user_agent = request.env['HTTP_USER_AGENT']
    
    if user_agent
      if user_agent.index('Windows')
        @os = 'windows'
      elsif user_agent.index('Macintosh')
        @os = 'apple'
      else
        @os = 'other-os'
      end
    end
  end
end

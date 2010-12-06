require 'test_helper'

class AudioItemsControllerTest < ActionController::TestCase
  test "should get index" do
    get :index
    assert_response :success
    assert_not_nil assigns(:audio_items)
  end

  test "should get new" do
    get :new
    assert_response :success
  end

  test "should create audio_item" do
    assert_difference('AudioItem.count') do
      post :create, :audio_item => { }
    end

    assert_redirected_to audio_item_path(assigns(:audio_item))
  end

  test "should show audio_item" do
    get :show, :id => audio_items(:one).to_param
    assert_response :success
  end

  test "should get edit" do
    get :edit, :id => audio_items(:one).to_param
    assert_response :success
  end

  test "should update audio_item" do
    put :update, :id => audio_items(:one).to_param, :audio_item => { }
    assert_redirected_to audio_item_path(assigns(:audio_item))
  end

  test "should destroy audio_item" do
    assert_difference('AudioItem.count', -1) do
      delete :destroy, :id => audio_items(:one).to_param
    end

    assert_redirected_to audio_items_path
  end
end

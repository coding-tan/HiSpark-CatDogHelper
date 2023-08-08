// pages/login/login.js
Page({

  /**
   * 页面的初始数据
   */
  data: {
   UserID:'',
   Password:'',
   defaultType:true,
   passwordType: true,
   userInfo:[],
   show:false //展示登录按钮
  },
  //defaultType:眼睛状态，显示密码和不显示密码
  eyeStatus:function eyeStatus() {
    if (this.data.defaultType) {
      this.setData({
        defaultType:false,
        passwordType: false
      })
    } else {
      this.setData({
        defaultType:true,
        passwordType: true
      })
    }
  },
  /* 获取用户输入的SSID*/
  bindUserIDInput:function(res){
    var that = this;
    that.setData({
      UserID:res.detail.value
    })
  },
  /*获取用户输入的password */
  bindPasswordInput:function(res){
    var that = this;
    that.setData({
      Password:res.detail.value
    })
  },
  /*点击将SSID和password传递给下一个页面进行wifi连接 */
  onClickSubmit:function () {
    var that = this;
    console.log("wifi名称"+that.data.WiFiSSID+"WiFi密码"+that.data.WiFiPassword);
    wx.navigateTo({
      url: '/pages/index/index?WIFI_SSID=' +that.data.WiFiSSID + '&WIFI_PASSWORD=' +that.data.WiFiPassword,
    })
  },

  onClickJump:function () {
    wx.navigateTo({
      url: '/pages/TencentCloud/TencentCloud'
    })
    wx.showToast({
      title: '登录成功',
      icon: 'none'
    })
  },
  /**
   * 生命周期函数--监听页面加载
   */
  onLoad: function (options) {

 },

  /**
   * 生命周期函数--监听页面初次渲染完成
   */
  onReady: function () {

  },

  /**
   * 生命周期函数--监听页面显示
   */
  onShow: function () {

  },

  /**
   * 生命周期函数--监听页面隐藏
   */
  onHide: function () {

  },
  
  /**
   * 生命周期函数--监听页面卸载
   */
  onUnload: function () {

  },

  /**
   * 页面相关事件处理函数--监听用户下拉动作
   */
  onPullDownRefresh: function () {

  },

  /**
   * 页面上拉触底事件的处理函数
   */
  onReachBottom: function () {

  },

  /**
   * 用户点击右上角分享
   */
  onShareAppMessage: function () {

  }
})
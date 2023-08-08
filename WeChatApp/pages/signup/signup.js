// pages/signup.js
Page({
  onSubmit: function (e) {
    const { username, password, confirmPassword } = e.detail.value;
    if (!username || !password || !confirmPassword) {
      wx.showToast({
        title: "请填写完整信息",
        icon: "none",
      });
      return;
    }
    if (password !== confirmPassword) {
      wx.showToast({
        title: "两次密码不一致",
        icon: "none",
      });
      return;
    }
    // 发送注册请求，注册成功后跳转到登录页
    wx.request({
      url: "注册接口地址",
      method: "POST",
      data: {
        username,
        password,
      },
      success: (res) => {
        wx.showToast({
          title: "注册成功",
          icon: "success",
        });
        wx.navigateTo({
          url: "/pages/login/login",
        });
      },
      fail: (err) => {
        wx.showToast({
          title: "注册失败",
          icon: "none",
        });
      },
    });
  },
 
  goToLogin: function () {
    wx.navigateTo({
      url: "/pages/login/login",
    });
  },
});
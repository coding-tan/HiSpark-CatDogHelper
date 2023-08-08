vlc获取hi3516端的rtsp视频流串流为webm格式输出
webm格式可以实现电脑手机网页播放(主要是手机网页不能播放ogg格式视频）
由于webm格式能在微信小程序调试端播放，Android实机上播放解析不了(表现为黑屏）
故，使用ffmpeg将webm视频格式转换为mp4格式(从vlc直接串流mp4播放不了）

ffmpeg -i video.webm -c:v libx264 -c:a aac video.mp4
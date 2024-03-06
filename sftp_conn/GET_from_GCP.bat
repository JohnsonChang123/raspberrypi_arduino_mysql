@echo on
(
echo get -r  /home/pi/ar0/video ar0
echo rm /home/pi/ar0/video
echo bye

)| sftp -i C:\Users\tony1\.ssh\id_rsa lab403jkc@35.194.132.151
(
echo sudo rm /home/pi/*
echo exit
)| ssh lab403jkc@35.194.132.151

while true; do
    clear -x
    xprop -root | grep -Po "WM_NAME\(STRING\) = \"\K.*(?=\")"
    sleep 0.5
done

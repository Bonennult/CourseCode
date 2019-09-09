w=fspecial('gaussian',[3 3],3);
% figure;
% hold on;
for i=4:4
    figure;
    hold on;
    img = imread([num2str(i),'.bmp']);
    img_lr = imfilter(img, w);
    img_ds = imresize(img_lr, 1/3);
    %a1 = subplot(1,3,(i-1)*3+1);
    a1 = subplot(1,3,1);
    imshow(img);
    title('original image');
    %a2 = subplot(3,3,(i-1)*3+2);
    a2 = subplot(1,3,2);
    imshow(img_lr);
    title('gaussian blurring');
    %a3 = subplot(3,3,(i-1)*3+3);
    a3 = subplot(1,3,3);
    imshow(img_ds);
    title('downsampling');
    imwrite(img_ds, [num2str(i),'_lr.bmp']);
    xsize = get(a1, 'XLim');
    ysize = get(a1, 'YLim');
    set(a2, 'XLim',xsize,'YLim',ysize);
    set(a3, 'XLim',xsize,'YLim',ysize);
end

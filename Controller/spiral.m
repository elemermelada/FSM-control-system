iters = 100;
loopStep = 10;


figure(1)
%hold on
%subplot(1,3,1);
%Spiral(50,3);
%axis equal
% subplot(1,3,2);
Spiral(1000,3);
axis equal
% subplot(1,3,3);
%Spiral(50,5);
%axis equal
print -dpdf sp2

function err = Spiral(iters,loopStep)
    x=[];
    y=[];
    for i = 1:iters
        r = 0.5*i/iters;
        th = 2.0*3.14159*loopStep*i/iters;
        x = [x r*cos(th)];
        y = [y r*sin(th)];
    end
    plot(x,y)

    err = 0;
end
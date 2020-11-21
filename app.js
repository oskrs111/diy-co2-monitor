let ip = wifi_module_get_ipaddress(1);
console.log('IP => 1', ip);

ip = wifi_module_get_ipaddress(2);
console.log('IP => 2', ip);

ip = wifi_module_get_ipaddress(3);
console.log('IP => 3', ip);

ip = wifi_module_get_ipaddress(4);
console.log('IP => 4', ip);


function wifi_module_get_ipaddress(dots)
{
    let ip = ['1','1','1','.','2','2','2','.','3','3','3','.','4','4','4',0x00];
    let t = 0;
    if(dots < 1) dots = 1;
    if(dots >= 4)
    {
        return ip;    
    }        
    dots--;
    for(t = (ip.length -1); t > 0; t--)
    {        
        if((ip[t] === '.') && (dots === 0x00))
        {
            return ip[t+1];
        }
        else if(ip[t] === '.')
        {
            dots--;
        }        
    }
    return ip;
}
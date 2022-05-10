
window.onload = function()
{
    document.getElementById('config1').onclick = loadConfig1;
    document.getElementById('config2').onclick = loadConfig2;
    document.getElementById('misc').onclick = loadMisc;
}

function loadUxConn()
{
    loadPage("uxconn.shtm");
    return false;
}

function loadConfig1()
{
    loadPage("config1.shtm");
    return false;
}

function loadConfig2()
{
    loadPage("config2.shtm");
    return false;
}

function loadMisc()
{
    loadPage("misc.shtm");
    return false;
}

function setInnerHTML(elm, html) {
  elm.innerHTML = html;
  Array.from(elm.querySelectorAll("script")).forEach( oldScript => {
    const newScript = document.createElement("script");
    Array.from(oldScript.attributes)
      .forEach( attr => newScript.setAttribute(attr.name, attr.value) );
    newScript.appendChild(document.createTextNode(oldScript.innerHTML));
    oldScript.parentNode.replaceChild(newScript, oldScript);
  });
}

function loadPage(page)
{
    if(window.XMLHttpRequest)
    {
        xmlhttp = new XMLHttpRequest();
    }
    else
    {
        xmlhttp = new ActiveXObject("Microsoft.XMLHTTP");
    }

    xmlhttp.open("GET", page, true);
    xmlhttp.setRequestHeader("Content-type",
                             "application/x-www-form-urlencoded");
    xmlhttp.send();

    xmlhttp.onreadystatechange = function ()
    {
        if((xmlhttp.readyState == 4) && (xmlhttp.status == 200))
        {
            setInnerHTML(document.getElementById("content"), xmlhttp.responseText);
        }
    }
}

function FindAndSelect(selectBox, value)
{
  var i = 0;
  for(i = 0; i < selectBox.length; i++)
  {
    if(selectBox.options[i].value == value)
    {
      selectBox.selectedIndex = i;
      return;
    }
  }
}

function FindNearestAndSelect(selectBox, value)
{
  var i = 0;
  var min;
  var max;

  for(i = 0; i < selectBox.length; i++)
  {
    min = (value * 90) / 100;
    max = (value * 110) / 100;

    if((selectBox.options[i].value > min) && (selectBox.options[i].value < max))
    {
      selectBox.selectedIndex = i;
      return;
    }
  }
}

function SetCfgFormDefaults()
{
  FindNearestAndSelect(document.config.br, br);
  FindAndSelect(document.config.stop, sb);
  FindAndSelect(document.config.dl, dl);
  FindAndSelect(document.config.flow, fc);
  FindAndSelect(document.config.parity, par);
  FindAndSelect(document.config.tnmode, tnm);
  FindAndSelect(document.config.tnprot, tnp);
  document.config.telnett.value = tt;
  document.config.telnetlp.value = tlp;
  document.config.telnetrp.value = trp;
  document.config.telnetip1.value =tip1;
  document.config.telnetip2.value =tip2;
  document.config.telnetip3.value =tip3;
  document.config.telnetip4.value =tip4;
  SetIPState();
}

function SetIPState()
{
  var tnmode = document.config.tnmode.value;
  var disable;
  if(tnmode == 0)
  {
    disable = true;
  }
  else
  {
    disable = false;
  }
  document.config.telnetip1.disabled = disable;
  document.config.telnetip2.disabled = disable;
  document.config.telnetip3.disabled = disable;
  document.config.telnetip4.disabled = disable;
  document.config.telnetrp.disabled = disable;
}

function CheckMiscDefaults(myForm)
{
  var answer = confirm("This will erase all existing configuration changes and restore factory default settings. Click OK if you are sure you want to do this or Cancel to retain existing settings.");
  if(answer == false)
  {
    alert("Existing configuration settings have been retained.");
  }
  return(answer);
}

function SetMiscIPState()
{
  var staticip = document.ip.staticip.value;
  var disable;
  if(staticip == 0)
  {
    disable = true;
  }
  else
  {
    disable = false;
  }
  document.ip.sip1.disabled = disable;
  document.ip.sip2.disabled = disable;
  document.ip.sip3.disabled = disable;
  document.ip.sip4.disabled = disable;
  document.ip.mip1.disabled = disable;
  document.ip.mip2.disabled = disable;
  document.ip.mip3.disabled = disable;
  document.ip.mip4.disabled = disable;
  document.ip.gip1.disabled = disable;
  document.ip.gip2.disabled = disable;
  document.ip.gip3.disabled = disable;
  document.ip.gip4.disabled = disable;
}

function SetMiscFormDefaults()
{
  FindAndSelect(document.ip.staticip, staticip);
  document.ip.sip1.value = sip1;
  document.ip.sip2.value = sip2;
  document.ip.sip3.value = sip3;
  document.ip.sip4.value = sip4;
  document.ip.mip1.value = mip1;
  document.ip.mip2.value = mip2;
  document.ip.mip3.value = mip3;
  document.ip.mip4.value = mip4;
  document.ip.gip1.value = gip1;
  document.ip.gip2.value = gip2;
  document.ip.gip3.value = gip3;
  document.ip.gip4.value = gip4;
  if(staticip == 1)
  {
    SetMiscIPState(false);
  }
  else
  {
    SetMiscIPState(true);
  }
}

const message = document.getElementById("message");
const button = document.getElementById("send");
const statusText = document.getElementById("status");
const result = document.getElementById("result");

button.addEventListener("click", async function()
{
    button.disabled = true;
    statusText.textContent = "正在发送...";
    result.textContent = "";

    try
    {
        const response = await fetch("/echo",
        {
            method: "POST",
            headers:
            {
                "Content-Type": "text/plain; charset=utf-8"
            },
            body: message.value
        });

        const body = await response.text();

        if(response.ok)
        {
            statusText.textContent = "发送成功，HTTP " + response.status;
        }
        else
        {
            statusText.textContent = "服务器返回错误，HTTP " + response.status;
        }

        result.textContent = body;
    }
    catch(error)
    {
        statusText.textContent = "请求失败：" + error.message;
    }
    finally
    {
        button.disabled = false;
    }
});
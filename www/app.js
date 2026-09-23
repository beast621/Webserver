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

const refreshButton = document.getElementById("refresh-status");
const activeConnections = document.getElementById("active-connections");
const pendingTasks = document.getElementById("pending-tasks");
const uptime = document.getElementById("uptime");
const serverStatusMessage = document.getElementById("server-status-message");

async function refreshStatus()
{
    refreshButton.disabled = true;
    serverStatusMessage.textContent = "正在读取...";

    try
    {
        const response = await fetch("/status", {
            cache: "no-store"
        });

        if(!response.ok)
        {
            throw new Error("HTTP " + response.status);
        }

        const data = await response.json();

        activeConnections.textContent = data.active_connections;
        pendingTasks.textContent = data.pending_tasks;
        uptime.textContent = data.uptime_seconds;

        serverStatusMessage.textContent = "刷新成功";
    }
    catch(error)
    {
        serverStatusMessage.textContent =
            "刷新失败，显示值可能已过期：" + error.message;
    }
    finally
    {
        refreshButton.disabled = false;
    }
}

refreshButton.addEventListener("click", refreshStatus);

// 页面打开时先读取一次。
refreshStatus();
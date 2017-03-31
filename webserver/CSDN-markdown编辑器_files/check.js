$.get("/account/check", function (data) {
    var str = data + "";
    if (str == 0)
    {
        location.href = "/postedit";
    }
});
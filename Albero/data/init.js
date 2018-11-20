document.addEventListener('DOMContentLoaded', () => {
    var url = new URL(location.href);
    var state = url.searchParams.get("state");
    document.querySelector(".tree").classList.add(state);
 }, false);
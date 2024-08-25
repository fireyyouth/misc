import { useState } from 'react'
import reactLogo from './assets/react.svg'
import viteLogo from '/vite.svg'
import './App.css'

interface PostData {
  id: string,
  date: string,
  content: string
}

function uuidv4() {
  return "10000000-1000-4000-8000-100000000000".replace(/[018]/g, c =>
    (+c ^ crypto.getRandomValues(new Uint8Array(1))[0] & 15 >> +c / 4).toString(16)
  );
}

function Post({data, RemovePost}) {
  let [op_visible, set_op_visible] = useState(false)
  let [comment_visible, set_comment_visible] = useState(false)

  function op_class() {
    return 'op ' + (op_visible? '' : 'invisible')
  }

  function toggle_comment() {
    set_comment_visible(!comment_visible)
  }

  return (
    <div className="post" onMouseOver={() => set_op_visible(true)} onMouseLeave={() => set_op_visible(false)}>
      <h4>{data.date}</h4>
      <p>{data.content}</p>
      <div className={op_class()}>
        <div onClick={() => RemovePost(data.id)}>
          delete
        </div>
        <div onClick={() => toggle_comment()}>
          comment
        </div>
      </div>
      {comment_visible &&
        <>
          <div>
            <input></input>
            <div>
              <button>comment</button>
            </div>
          </div>
          <div>
            {data.comments.map(c => <div key={uuidv4()}>bot: {c}</div>)}
          </div>
        </>
      }
    </div>
  )
}

function App() {
  const [post_list, set_post_list] = useState([
    {
      id: uuidv4(),
      date: '10.01',
      content: 'today is a good day',
      comments: [
        "good job",
        "me too"
      ]
    },
    {
      id: uuidv4(),
      date: '10.02',
      content: 'today is a bad day',
      comments: [
        "feel sorry for you",
      ]
    }
  ])

  let [stage, set_stage] = useState('')

  function delete_post(post_id: string) {
    let cp = Array.from(post_list)
    for (let i = 0; i < cp.length; ++i) {
      if (cp[i].id == post_id) {
        cp.splice(i, 1)
        break
      }
    }
    set_post_list(cp)
  }

  function stage_to_post() {
    let cp = Array.from(post_list)
    cp.push({
      id: uuidv4(),
      date: `10.${Math.floor(Math.random() * 10)}`,
      content: stage,
      comments: []
    })
    set_stage('')
    set_post_list(cp)
  }

  let post_items = post_list.map((e) => <Post key={e.id} data={e} RemovePost={delete_post} ></Post>)

  return (
    <>
      <div className="panel">
        {post_items}
      </div>
      <div className="stage">
        <textarea value={stage} onChange={e => set_stage(e.target.value)}></textarea>
        <button onClick={() => stage_to_post()}>append</button>
      </div>
    </>
  )
}

export default App
